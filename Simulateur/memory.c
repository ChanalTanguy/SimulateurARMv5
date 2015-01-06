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
#include <stdlib.h>
#include "memory.h"
#include "util.h"

#define masque_8 128

struct memory_data {
    int8_t *address;
    size_t size;
};

memory memory_create(size_t size) {
    memory mem;

    mem = malloc(sizeof(struct memory_data));
    if (mem) {
        mem->address = malloc(size);
        if (!mem->address) {
            free(mem);
            mem = NULL;
        }
    }
    if (mem) {
        mem->size = size;
    }
    return mem;
}

size_t memory_get_size(memory mem) {
    return mem->size;
}

void memory_destroy(memory mem) {
    free(mem->address);
    free(mem);
}

int memory_read_byte(memory mem, uint32_t address, uint8_t *value) {
    if (address > (mem->size)-1){
		return -1;
	}
	*value = *(mem->address + address);
	return 0;
}

int memory_read_half(memory mem, int be, uint32_t address, uint16_t *value) {
    if (address > (mem->size)-2){
		return -1;
	}
	*value = *(mem->address + address + 1)<<8;
	*value += *(mem->address + address);
	if (be == 1)  {
		*value = reverse_2(*value);
	} else if (be != 0) {
		return -1;
	}
	return 0;
}

int memory_read_word(memory mem, int be, uint32_t address, uint32_t *value) {
  
  if ( (address+3) > (mem->size)-1){
    return -1;
  }
  else 
    {
 
      *value = *(mem->address + address + 3)<<24;
      *value += *(mem->address + address + 2)<<16;
      *value += *(mem->address + address + 1)<<8;
      *value += *(mem->address + address);

      if ( be == 1)  
	{
	  *value=reverse_4(*value);
	}   
    }
  return 0;
}

int memory_write_byte(memory mem, uint32_t address, uint8_t value) {
    if (address > (mem->size)-1){
		return -1;
	}
    mem->address[address]= value;
    return 0;
}

int memory_write_half(memory mem, int be, uint32_t address, uint16_t value) {

  uint8_t val;

  if ( (address+1) > (mem->size)-1){
		return -1;
	}
  else
    {
      if ( be==0)
      {mem->address[address]= value;
	val = value>>8;
	mem->address[address+1]= val;
      }
      else {
	val = value>>8;
	memory_write_byte( mem, address, val );
	val = value;
	memory_write_byte( mem, (address+1), val );
      }
    }
  return 0;
}

int memory_write_word(memory mem, int be, uint32_t address, uint32_t value) {
  
   uint8_t val;
  
  if ( (address+3) > (mem->size)-1){
    return -1;
  }
  else 
    {
      if (be==0)
	{
	  *(mem->address+address)=value;
	}
      else 
	{
	  //  *(mem->address+address)=reverse_4(value);

	  val = value >> 24 ;
	  memory_write_byte( mem, address, val );
	  val = value >> 16 ;
	  memory_write_byte( mem, address+1, val );
	  val = value >> 8 ;
	  memory_write_byte( mem, address+2, val );
	  val = value ;
	  memory_write_byte( mem, address+3, value );

}
      
    }
  return 0;
}
