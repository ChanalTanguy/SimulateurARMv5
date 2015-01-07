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
	uint32_t adr_pc;
	adr_pc = arm_read_register(p, 15); // On récupère l'adresse contenue dans le PC (15 = Registre correspondant au PC)
	if (arm_read_word(p, adr_pc, &instr) != 0){ // On lit l'instruction contenue à l'adresse récupérée
		return -1;
	}
	arm_write_register(p, 15, adr_pc+2); // On incrémente l'adresse contenue dans le PC de 4

	uint8_t cond;

	uint32_t flags = arm_read_cpsr(p); // Récupération du CPSR 
	
	// Le CPSR contient les flags ZNCV : http://www.heyrick.co.uk/armwiki/The_Status_register
	
	int n = get_bit(flags, 31);
	int z = get_bit(flags, 30);
	int c = get_bit(flags, 29);
	int v = get_bit(flags, 28);

	cond = get_bits(instr, 31, 28); // Récupération du code condition de l'instruction
	int res;
	
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

	if (res) {
		// Exécuter l'instruction
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
