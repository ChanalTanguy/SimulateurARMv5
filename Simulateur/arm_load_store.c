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
  uint32_t addresse;
  uint32_t value;
  uint32_t offset=0;

  rn = gets_bits(ins,19,16);
  rd = gets_bits(ins,15,12);
  adresse = arm_read_register( p,rn);
value = arm_read_register(p,rd);
  masque = get_bits(ins,27,26);
  
if (masque == 0)
    { //ldrH et strH
      if (get_bit(ins,20) == 0)
		{ //strH
	  
		}
	else
	{ // ldrH
	  
	}
    }
  else{
  	 // ldr, ldrb, str, strb
		if (get_bit(ins,20) == 0)//L
		{ //str et strb 
			if (get_bit(ins,22) == 0) //B
	    {//str	
		if (get_bit(ins,25)==0)// I
			{ offset = gets_bits(ins,11,0);
			if ( get_bit(ins,23)==0)//U
				{ adresse =adresse-offset;}
			else 
				{adresse =adresse+offset;}
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
					if ( get_bit(ins,23)==0)//U
						{ adresse =adresse-offset;}
					else 
						{adresse =adresse+offset;}
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
		      adresse = arm_read_register( p,rn);
		      value = arm_read_register(p,rd);
		      offset = arm_read_register( p,rm);
		      if ( get_bit(ins,23)==0)//U
			{ adresse =adresse-offset;}
		      else 
			{adresse =adresse+offset;}
		      arm_write_word( p,address,value);
		    }
		  else
		    {// W= 1 et P=1 pre index addressing
		      rm = gets_bits(ins,3,0);
		      adresse = arm_read_register( p,rn);
		      value = arm_read_register(p,rd);
		      offset = arm_read_register( p,rm);
		      if ( get_bit(ins,23)==0)//U
			{ adresse =adresse-offset;}
		      else 
			{adresse =adresse+offset;}
		      rn = arm_write_register(p,adresse);
		      arm_write_word( p,address,value);
		    }
		}
		}
	      else
		{//strb
		  if  (get_bit(ins,24)==0)// P
		    {//str post indexed adressing
		      if (get_bit(ins,21)==0)//W
			{// str normal memory acces(
			  adresse = arm_read_register( p,rn);
			  value = arm_read_register(p,rd);
			  offset = gets_bits(ins,11,0);
			  arm_write_bytes( p,address,value);
			  if ( get_bit(ins,23)==0)//U
			    { adresse =adresse-offset;}
			  else 
			    {adresse =adresse+offset;}
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
			  adresse = arm_read_register( p,rn);
			  value = arm_read_register(p,rd);
			  offset = arm_read_register( p,rm);
			  if ( get_bit(ins,23)==0)//U
			    { adresse =adresse-offset;}
			  else 
			    {adresse =adresse+offset;}
			  arm_write_byte( p,address,value);
			}
		      else
			{// W= 1 et P=1 pre index addressing
			  rm = gets_bits(ins,3,0);
			  adresse = arm_read_register( p,rn);
			  value = arm_read_register(p,rd);
			  offset = arm_read_register( p,rm);
			  if ( get_bit(ins,23)==0)//U
			    { adresse =adresse-offset;}
			  else 
			    {adresse =adresse+offset;}
			  rn = arm_write_register(p,adresse);
			  arm_write_byte( p,address,value);
			}
		    }
		}
	    }
	}
      else
	{ // ldr et ldrb
	  masque = get_bit(ins,22);
	  if (masque == 0)
	    {//ldr
	    }
	  else
	    {//ldrb
		  
	    }
	}
    }
  
  return UNDEFINED_INSTRUCTION;
}

int offset_up

int arm_load_store_multiple(arm_core p, uint32_t ins) {
  return UNDEFINED_INSTRUCTION;
}
  
int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
  /* Not implemented */
  return UNDEFINED_INSTRUCTION;
}
