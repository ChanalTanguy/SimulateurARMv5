/*
Armator - simulateur de jeu d'instruction ARMv5T à but pédagogique
Copyright (C) 2011 Guillaume Huard
Ce programme est libre, vous pouvez le redistribuer et/ou le modifier selon les
termes de la Licence Publique Générale GNU publiée par la Free Software
Foundation (version 2 ou bien toute autre version ultérieure choisie par vous).

Ce programme est distribué car potentiellement utile, mais SANS AUCUNE
GARANTIE, ni explicite ni implicite, y compris les garanties de
commercialisation ou d'adaptation dans un but spécifique. Reportez-vous à la
Licence Publique Générale GNU pour plus de détails.

Vous devez avoir reçu une copie de la Licence Publique Générale GNU en même
temps que ce programme ; si ce n'est pas le cas, écrivez à la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
États-Unis.

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
  uint32_t addresse;
  uint32_t value;
  uint32_t offset=0;

	rn = get_bits(ins,19,16);
	rd = get_bits(ins,15,12);
	adresse = arm_read_register( p,rn);
	value = arm_read_register(p,rd);
	masque = get_bits(ins,27,26);
  
if (get_bits(ins,27,26) == 0)
    { //ldrH et strH
		if(get_bit(ins,22)==0)
		{}
		else
		{}
     
    }
  else{
  	 // ldr, ldrb, str, strb
		if (get_bit(ins,20) == 0)//L
		{ //str et strb 
			if (get_bit(ins,22) == 0) //B
			{//str	
				if (get_bit(ins,25)==0)// I
					{ 
					offset = gets_bits(ins,11,0);
					offset_op(ins,adresse);
					arm_write_word(p,rn,adresse);
					}
				else 
					{
					if (get_bit(ins,24)==0)// P
						{//str post indexed adressing
						if (get_bit(ins,21)==0)//W
							{// str normal memory acces
							offset = gets_bits(ins,11,0);
							arm_write_word( p,address,value);
							offset_op(ins,adresse);
							arm_write_register(p,rn,adresse);
							}
						else 
						{return UNDEFINED_INSTRUCTION; }
						}
					else//p==1
						{
						if ( get_bit(ins,21)==0)//W
							{//offset adressing
							rm = gets_bits(ins,3,0);
							offset = arm_read_register( p,rm);
							offset_op(ins,adresse);
							arm_write_word( p,address,value);
							}
						else
							{// W= 1 et P=1 pre index addressing
							rm = gets_bits(ins,3,0);
							offset = arm_read_register( p,rm);
							offset_op(ins,adresse);
							rn = arm_write_register(p,adresse);
							arm_write_word( p,address,value);
							}
						}
					}
	      else
			{//strb
				if (get_bit(ins,25)==0)// I
					{ 
					offset = gets_bits(ins,11,0);
					offset_op(ins,adresse);
					arm_write_byte(p,rn,adresse);
					}
				else
					{
					if  (get_bit(ins,24)==0)// P
						{//str post indexed adressing
						if (get_bit(ins,21)==0)//W
							{// str normal memory acces(
							offset = gets_bits(ins,11,0);
							arm_write_bytes( p,address,value);
							offset_op(ins,adresse);
							arm_write_register(p,rn,adresse);
							}
						else 
							{return UNDEFINED_INSTRUCTION; }
						}
					else//p==1
						{
						if ( get_bit(ins,21)==0)//W
							{//offset adressing
							rm = gets_bits(ins,3,0);
							offset = arm_read_register( p,rm);
							offset_op(ins,adresse);
							arm_write_byte( p,address,value);
							}
						else
							{// W= 1 et P=1 pre index addressing
							rm = gets_bits(ins,3,0);
							offset = arm_read_register( p,rm);
							offset_op(ins,adresse);
							rn = arm_write_register(p,adresse);
							arm_write_byte( p,address,value);
							}
						}
					}
			}
			}
		else
			{ //ldr et ldrb 
				if (get_bit(ins,22) == 0) //B
					{//ldr	
					if (get_bit(ins,25)==0)// I
						{ 
						offset = gets_bits(ins,11,0);
						offset_op(ins,adresse);
						arm_write_word(p,rn,adresse);
						}
					else 
						{
						if (get_bit(ins,24)==0)// P
							{//ldr post indexed adressing
							if (get_bit(ins,21)==0)//W
								{// ldr normal memory acces
								offset = gets_bits(ins,11,0);
								arm_write_word( p,address,value);
								offset_op(ins,adresse);
								arm_write_register(p,rn,adresse);
								}
							else 
								{return UNDEFINED_INSTRUCTION; }
							}
						else//p==1
							{
								if ( get_bit(ins,21)==0)//W
									{//offset adressing
									rm = gets_bits(ins,3,0);
									offset = arm_read_register( p,rm);
									offset_op(ins,adresse);
									arm_write_word( p,address,value);
									}
								else
									{// W= 1 et P=1 pre index addressing
									rm = gets_bits(ins,3,0);
									offset = arm_read_register( p,rm);
									offset_op(ins,adresse);
									rn = arm_write_register(p,adresse);
									arm_write_word( p,address,value);
									}
							}
						}
					}
				else
					{//ldrb
					if (get_bit(ins,25)==0)// I
						{ 
						offset = gets_bits(ins,11,0);
						offset_op(ins,adresse);
						arm_write_byte(p,rn,adresse);
						}
					else
						{
						if  (get_bit(ins,24)==0)// P
							{//ldrb post indexed adressing
							if (get_bit(ins,21)==0)//W
								{// ldrb normal memory acces(
								offset = gets_bits(ins,11,0);
								arm_write_bytes( p,address,value);
								offset_op(ins,adresse);
								arm_write_register(p,rn,adresse);
								}
							else 
								{return UNDEFINED_INSTRUCTION; }
							}
						else//p==1
							{
								if ( get_bit(ins,21)==0)//W
									{//offset adressing
									rm = gets_bits(ins,3,0);
									offset = arm_read_register( p,rm);
									offset_op(ins,adresse);
									arm_write_byte( p,address,value);
									}
								else
									{// W= 1 et P=1 pre index addressing
									rm = gets_bits(ins,3,0);
									offset = arm_read_register( p,rm);
									offset_op(ins,adresse);
									rn = arm_write_register(p,adresse);
									arm_write_byte( p,address,value);
									}
							}
						}
					}
			}
}

uint32_t offset_op(uint32_t ins,uint32_t adresse){
				if ( get_bit(ins,23)==0)//U
				{  return adresse-offset;}
			else 
				{return adresse+offset;}
			}

int arm_load_store_multiple(arm_core p, uint32_t ins) {
  return UNDEFINED_INSTRUCTION;
}
  
int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
  /* Not implemented */
  return UNDEFINED_INSTRUCTION;
}
