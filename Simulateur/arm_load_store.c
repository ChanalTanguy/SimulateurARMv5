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
  masque = get_bits(ins,27,26);
  if (masque == 0)
    { //ldrH et strH
      masque = get_bit(ins,20);
      if (masque == 0)
	{ //strH
	
	}

      else
	{ // ldrH
	  
	}
    }
  else
    { // ldr, ldrb, str, strb
       masque = get_bit(ins,20);
       if (masque == 0)
	{ //str et strb
	  masque = get_bit(ins,22);
	  if (masque == 0)
	    {//str
	    }
	  else
	    {//strb
	      
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

int arm_load_store_multiple(arm_core p, uint32_t ins) {
    return UNDEFINED_INSTRUCTION;
}

int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
    /* Not implemented */
    return UNDEFINED_INSTRUCTION;
}
