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
#include "arm_instruction.h"
#include "arm_exception.h"
#include "arm_data_processing.h"
#include "arm_load_store.h"
#include "arm_branch_other.h"
#include "arm_constants.h"
#include "util.h"

static int arm_execute_instruction(arm_core p) {
    	uint32_t instr;
	if (arm_fetch(p, &instr) == -1) return -1;		
	uint8_t cond;

	uint32_t flags = arm_read_cpsr(p); // Récupération du CPSR 
	
	// Le CPSR contient les flags ZNCV : http://www.heyrick.co.uk/armwiki/The_Status_register
	
	int n = get_bit(flags, 31);
	int z = get_bit(flags, 30);
	int c = get_bit(flags, 29);
	int v = get_bit(flags, 28);
	
	cond = get_bits(instr, 31, 28); // Récupération du code condition de l'instruction
	int res = 0;
	
	switch (cond) {
		case 0 : res = (z == 1); break;
		case 1 : res = (z == 0); break;
		case 2 : res = (c == 1); break;
		case 3 : res = (c == 0); break;
		case 4 : res = (n == 1); break;
		case 5 : res = (n == 0); break;
		case 6 : res = (v == 1); break;
		case 7 : res = (v == 0); break;
		case 8 : res = ((c == 1) && (z == 0)); break;
		case 9 : res = ((c == 0) || (z == 1)); break;
		case 10 : res = (((n == 1) && (v == 1)) || ((n == 0) && (v == 0))); break;
		case 11 : res = (((n == 1) && (v == 0)) || ((n == 0) && (v == 1))); break;
		case 12 : res = ((z == 0) && (((n == 1) && (v == 1)) || ((n == 0) && (v == 0)))); break;
		case 13 : res = ((z == 1) || (((n == 1) && (v == 0)) || ((n == 0) && (v == 1)))); break;
		case 14 : res = 1; break;
		case 15 : /* DO SOMETHING FUNNY */ break;
		default : return -1;
	}

	uint8_t champ = get_bits(instr, 27, 25);

	if (res) {
		switch (champ) {
			case 0 : 
				if (get_bits(instr,24,23) == 2) {
					//Miscallenous instructions
					arm_miscellaneous(p, instr);
				} else {
					if (get_bit(instr, 4) == 0) {
						// Data processing immediate shift
						arm_data_processing_shift(p, instr);
					} else if (get_bit(instr, 7) == 0) {
						// Data processing register shift
						arm_data_processing_shift(p, instr);
					} else if (get_bit(instr, 7) == 1) {
						// Multiplies / Extra load/stores
						arm_load_store(p, instr);
					} else {
						return -1;
					}
				}
				break;
			case 1 :
				if ((get_bits(instr,24,23) == 2) && (get_bits(instr,21,20) == 0)) {
					// Undefined instruction
				} 
				else 	// Move immediate to status register
					arm_data_processing_immediate_msr(p, instr);
				break;
			case 2 : // Load/store immediate offset
				arm_load_store(p, instr);
				break;
			case 3 : if (get_bit(instr,4) == 0) {
						// Load/store register offset
						arm_load_store(p, instr);
					} else if ((get_bits(instr,24,20) == 31) && (get_bits(instr,7,4) == 15)) {
						// Architecturally undefined
					} else {
						// Media instructions
					}
				break;
			case 4 : // Load/store multiple
				break;
			case 5 : // Branch and branch with link
				 arm_branch(p, instr); break;
			case 6 : // Coprocessor load/store and double register transfer
				break;
			case 7 : if (get_bit(instr, 24) == 1) {
						// Software interrupt
						arm_coprocessor_others_swi(p, instr);
					} else {
						if (get_bit(instr,4) == 0) {
							// Coprocessor data processing
						} else { 
							// Coprocessor register transfers
						}
					}
				break;
			default : return -1; break;
		}
	}
	
	return 0;
}

int arm_step(arm_core p) {
    int result;

    result = arm_execute_instruction(p);
    if (result)
        arm_exception(p, result);
    return result;
}
