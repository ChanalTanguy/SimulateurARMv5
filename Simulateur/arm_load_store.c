/*
Armator - simulateur de jeu d'instruction ARMv5T � but p�dagogique
Copyright (C) 2011 Guillaume Huard
Ce programme est libre, vous pouvez le redistribuer et/ou le modifier selon les
termes de la Licence Publique G�n�rale GNU publi�e par la Free Software
Foundation (version 2 ou bien toute autre version ult�rieure choisie par vous).

Ce programme est distribu� car potentiellement utile, mais SANS AUCUNE
GARANTIE, ni explicite ni implicite, y compris les garanties de
commercialisation ou d'adaptation dans un but sp�cifique. Reportez-vous � la
Licence Publique G�n�rale GNU pour plus de d�tails.

Vous devez avoir re�u une copie de la Licence Publique G�n�rale GNU en m�me
temps que ce programme ; si ce n'est pas le cas, �crivez � la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
�tats-Unis.

Contact: Guillaume.Huard@imag.fr
         ENSIMAG - Laboratoire LIG
         51 avenue Jean Kuntzmann
         38330 Montbonnot Saint-Martin
*/
#include "arm_load_store.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "util.h"
#include "debug.h"

int arm_load_store(arm_core p, uint32_t ins) {
	uint8_t masque = 0;
	uint8_t rn;
	uint8_t rd;
	uint8_t rm;
	uint8_t shit;
	uint32_t addresse;
	uint32_t value;
	uint32_t offset=0;
	
	
	rn = get_bits(ins,19,16);
	rd = get_bits(ins,15,12);
	adresse = arm_read_register( p,rn);
	value = arm_read_register(p,rd);

if (get_bits(ins,27,26) == 0)
	{ //ldrH et strH
	}
else
	{ // ldr, ldrb, str, strb
	if (get_bit(ins,25)==0)// I
		{offset = get_bits(ins,11,0);}
	else 
		{
		rm = gets_bits(ins,3,0);
		offset = arm_read_register( p,rm);
			if ( get_bit(ins,11,4)!=0 )
			{ shit =get_bit(ins,6,5);
			}
			
		}
	if (get_bit(ins,24)==0)// P
		{//P=0 post indexed adressing
			if (get_bit(ins,21)==0)//W
				{//W=0 normal memory acces
					if (get_bit(ins,20) == 0)//L
						{// L=0 str et strb
						if (get_bit(ins,22) == 0) //B	
							{//B=0 str
							arm_write_word( p,address,value);
							}
							else
							{// B=1 strb
							arm_write_byte( p,address,value);
							}
					else 
					{ // L=1 ldr et ldrb
						if (get_bit(ins,22) == 0) //B	
							{//B=0 ldr
							arm_read_word( p,address,value);
							}
							else
							{// B=1 ldrb
							arm_read_byte( p,address,value);
							}
					}
				}
			else
			{// W=1 
				return UNDEFINED_INSTRUCTION; 
			}
		
		adresse=offset_op(ins,adresse,offset);
		arm_write_register(p,rn,adresse);		
		}
	else 
		{//P=1 offset adressing ou Pre-adrressing
		if (get_bit(ins,21)==0)//W
			{//W=0 offset adressing
			adresse=offset_op(ins,adresse,offset);
			if (get_bit(ins,20) == 0)//L
				{// L=0 str et strb
				if (get_bit(ins,22) == 0) //B	
					{//B=0 str
					arm_write_word( p,address,value);
					}
				else
					{// B=1 strb
						arm_write_byte( p,address,value);
						}
				}
			else 
				{ // L=1 ldr et ldrb
				if (get_bit(ins,22) == 0) //B	
					{//B=0 ldr
					arm_read_word( p,address,value);
					}
				else
					{// B=1 ldrb
					arm_read_byte( p,address,value);
					}
				}
			}
		else
			{// W=1 pre-addressing
			adresse=offset_op(ins,adresse,offset);
			arm_write_register(p,rn,adresse);
			if (get_bit(ins,20) == 0)//L
				{// L=0 str et strb
				if (get_bit(ins,22) == 0) //B	
					{//B=0 str
					arm_write_word( p,address,value);
					}
				else
					{// B=1 strb
					arm_write_byte( p,address,value);
					}
				}
			else 
				{ // L=1 ldr et ldrb
				if (get_bit(ins,22) == 0) //B	
					{//B=0 ldr
					arm_read_word( p,address,value);
					}
				else
					{// B=1 ldrb
					arm_read_byte( p,address,value);
					}
				}
			} 
		}		
	}
//fin ldr,ldrb, str, strb	
}
    return 1;
}

int arm_load_store_multiple(arm_core p, uint32_t ins) {
    return UNDEFINED_INSTRUCTION;
}

int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
    /* Not implemented */
    return UNDEFINED_INSTRUCTION;
}

uint32_t offset_op(uint32_t ins,uint32_t adresse,uint32_t offset){
	if ( get_bit(ins,23)==0)//U
		{  return adresse-offset;}
	else 
		{return adresse+offset;}
}


