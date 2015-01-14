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
#include "arm_data_processing.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_branch_other.h"
#include "util.h"
#include "debug.h"

/* Decoding functions for different classes of instructions */
int arm_data_processing_shift(arm_core p, uint32_t ins) {
	
	uint8_t opcode = get_bits(ins,24,21);
	uint8_t Rd = get_bits(ins,15,12);
	uint8_t Rn = get_bits(ins,19,16);
	//uint8_t I = get_bit(ins,25);
	uint8_t S = get_bit(ins,20);
	//uint8_t cond = get_bits(ins,31,28);
	uint32_t shifter_operand = get_bits(ins,11,0);
	uint32_t result;
	uint8_t Rm = get_bits(ins,3,0);
	uint8_t shift_imm = get_bits(ins,11,7);
	int shifter_carryout;
	uint8_t Rs = get_bits(ins,11,8);
	uint32_t alu_out;

	if(get_bit(ins,4)){
		switch(get_bits(ins,6,5)){
			case(0):
				/*logical shift left by register*/

				if((get_bits(arm_read_register(p,Rs),7,0)) == 0){
					shifter_operand = arm_read_register(p,Rm);
					shifter_carryout = get_bit(arm_read_cpsr(p),C); 
				}else if((get_bits(arm_read_register(p,Rs),7,0)) < 32){
					shifter_operand = (arm_read_register(p,Rm) << (get_bits(arm_read_register(p,Rs),7,0)));
					shifter_carryout = get_bit(arm_read_register(p,Rm),(32 - (get_bits(arm_read_register(p,Rs),7,0))));
				}else if((get_bits(arm_read_register(p,Rs),7,0)) == 32){
					shifter_operand = 0;
					shifter_carryout = get_bit(arm_read_register(p,Rm),0);
				}else{
					shifter_operand = 0;
					shifter_carryout = 0;
				}
				break;
			case(1):
				/*logical shift right by register*/

				if((get_bits(arm_read_register(p,Rs),7,0)) == 0){
					shifter_operand = arm_read_register(p,Rm);
					shifter_carryout = get_bit(arm_read_cpsr(p),C); 
				}else if((get_bits(arm_read_register(p,Rs),7,0)) < 32){
					shifter_operand = (arm_read_register(p,Rm) >> (get_bits(arm_read_register(p,Rs),7,0)));
					shifter_carryout = get_bit(arm_read_register(p,Rm),(get_bits(arm_read_register(p,Rs),7,0) - 1));
				}else if((get_bits(arm_read_register(p,Rs),7,0)) == 32){
					shifter_operand = 0;
					shifter_carryout = get_bit(arm_read_register(p,Rm),31);
				}else{
					shifter_operand = 0;
					shifter_carryout = 0;
				}
				break;
			case(2):
				/*arithmetic shift right by register*/

				if((get_bits(arm_read_register(p,Rs),7,0)) == 0){
					shifter_operand = arm_read_register(p,Rm);
					shifter_carryout = get_bit(arm_read_cpsr(p),C);
				}else if((get_bits(arm_read_register(p,Rs),7,0)) < 32){
					shifter_operand = asr(arm_read_register(p,Rm),get_bits(arm_read_register(p,Rs),7,0));
					shifter_carryout = get_bit(arm_read_register(p,Rm),(get_bits(arm_read_register(p,Rs),7,0) - 1));
				 }else{
					if(get_bit(arm_read_register(p,Rm),31) == 0){
					shifter_operand = 0;
					shifter_carryout = get_bit(arm_read_register(p,Rm),31);
					}else{
						shifter_operand = 0xFFFFFFFF;
						shifter_carryout = get_bit(arm_read_register(p,Rm),31);
					}
				}
				break;
				
			case(3):
				/* Rotate right by register*/

				if((get_bits(arm_read_register(p,Rs),7,0)) == 0){
					shifter_operand = arm_read_register(p,Rm);
					shifter_carryout = get_bit(arm_read_cpsr(p),C);
				}else if((get_bits(arm_read_register(p,Rs),4,0)) == 0){
					shifter_operand = arm_read_register(p,Rm);
					shifter_carryout = get_bit(arm_read_register(p,Rm),31);
				}else{
					shifter_operand = ror(arm_read_register(p,Rm),get_bits(arm_read_register(p,Rs),4,0));
					shifter_carryout = get_bit(arm_read_register(p,Rm),(get_bits(arm_read_register(p,Rs),4,0) - 1));
				}
				break;
		}
	}else{
		switch(get_bits(ins,6,5)){
			case(0):
				/*logical shift left by immediate*/

				if(shift_imm == 0){
					shifter_operand = arm_read_register(p,Rm);
					shifter_carryout = get_bit(arm_read_cpsr(p),C);
				}else{
					shifter_operand = (arm_read_register(p,Rm) << shift_imm);
					shifter_carryout = get_bit(arm_read_register(p,Rm),(32 - shift_imm));
				}
				break;
			case(1):
				/*logical shift right by immediate*/

				if(shift_imm == 0){
					shifter_operand = 0;
					shifter_carryout = get_bit(arm_read_register(p,Rm),31);
				}else{
					shifter_operand = (arm_read_register(p,Rm) >> shift_imm);
					shifter_carryout = get_bit(arm_read_register(p,Rm),(shift_imm-1));
				}
				break;
			case(2):
				/*arithmetic shift right by immediate*/

				if(shift_imm == 0){
					if(get_bit(arm_read_register(p,Rm),31) == 0){
						shifter_operand = 0;
						shifter_carryout = get_bit(arm_read_register(p,Rm),31);
					}else{
						shifter_operand = 0xFFFFFFFF;
						shifter_carryout = get_bit(arm_read_register(p,Rm),31);
					}
				}else{
					shifter_operand = asr(arm_read_register(p,Rm), shift_imm);
					shifter_carryout = get_bit(arm_read_register(p,Rm),(shift_imm - 1));
				}
				break;
			case(3):
				
				/* Rotate right by immediate*/

				if(shift_imm == 0){
					result = (get_bit(arm_read_cpsr(p),C) << 31) | (arm_read_register(p,Rm) >> 1);
					shifter_carryout = get_bit(arm_read_register(p,Rm),0);
				}else{
					shifter_operand = ror(arm_read_register(p,Rm),shift_imm);
					shifter_carryout = get_bit(arm_read_register(p,Rm),(shift_imm - 1));
				}

				break;
		}

	}

	switch(opcode){
		case(0): //AND
			result = arm_read_register(p, Rn) & shifter_operand; 
			arm_write_register(p, Rd, result);  //Rd = Rn AND shifter_operand
			if (S && (arm_read_register(p, Rd) == arm_read_register(p, 15))){ //if S == 1 and Rd == R15
				if(arm_current_mode_has_spsr(p)){
					arm_write_cpsr(p, arm_read_spsr(p));    //CPSR = SPSR
				}
			}else if(S){
				if(get_bit(arm_read_register(p, Rd),31)){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),N));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),N));
				
				if(!(arm_read_register(p, Rd))){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),Z));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),Z));
				
				if(shifter_carryout){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),C));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),C));
			}
			break;
		
		case(1): //EOR
			result = arm_read_register(p, Rn) ^ shifter_operand;
			arm_write_register(p, Rd, result);
			if (S && (arm_read_register(p, Rd) == arm_read_register(p, 15))){ //if S == 1 and Rd == R15
				if(arm_current_mode_has_spsr(p)){
					arm_write_cpsr(p, arm_read_spsr(p));    //CPSR = SPSR
				}
			}else if(S){
				if(get_bit(arm_read_register(p, Rd),31)){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),N));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),N));
				
				if(!(arm_read_register(p, Rd))){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),Z));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),Z));
				
				if(shifter_carryout){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),C));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),C));
			}
			break;
			 		
		case(2): //SUB
			result = arm_read_register(p, Rn) - shifter_operand;
			arm_write_register(p, Rd, result);
			if (S && (arm_read_register(p, Rd) == arm_read_register(p, 15))){ //if S == 1 and Rd == R15
				if(arm_current_mode_has_spsr(p)){
					arm_write_cpsr(p, arm_read_spsr(p));    //CPSR = SPSR
				}
			}else if(S){
				if(get_bit(arm_read_register(p, Rd),31)){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),N));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),N));
				
				if(!(arm_read_register(p, Rd))){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),Z));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),Z));
				
				if(BorrowFrom(arm_read_register(p, Rn),shifter_operand,0)){ //Borrow is on
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),C));
				}else 
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),C));

				if((get_bit(arm_read_register(p, Rn),31) != get_bit(shifter_operand,31)) && (get_bit(shifter_operand,31) == get_bit(arm_read_register(p, Rn)-shifter_operand,31))){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),V));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),V));
			}
			break;
			
		case(3): //RSB

			result = shifter_operand - arm_read_register(p, Rn);
			arm_write_register(p, Rd, result);
			if (S && (arm_read_register(p, Rd) == arm_read_register(p, 15))){ //if S == 1 and Rd == R15
				if(arm_current_mode_has_spsr(p)){
					arm_write_cpsr(p, arm_read_spsr(p));    //CPSR = SPSR
				}
			}else if(S){
				if(get_bit(arm_read_register(p, Rd),31)){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),N));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),N));
				
				if(!(arm_read_register(p, Rd))){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),Z));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),Z));
				
				if(BorrowFrom(shifter_operand,arm_read_register(p, Rn),0)){ //Borrow is on
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),C));
				}else 
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),C));

				if((get_bit(shifter_operand,31) != get_bit(arm_read_register(p, Rn),31)) && (get_bit(arm_read_register(p, Rn),31) == get_bit(shifter_operand-arm_read_register(p, Rn),31))){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),V));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),V)); 
			}
			break;
		
		case(4): //ADD
			result = arm_read_register(p,Rn) + shifter_operand;
			arm_write_register(p, Rd, result); //Rd = Rn + shifter_operand

			if (S && (arm_read_register(p, Rd) == arm_read_register(p, 15))){
				if(arm_current_mode_has_spsr(p)){
					arm_write_cpsr(p, arm_read_spsr(p));    //CPSR = SPSR
				}
			}else if(S){
				if(get_bit(arm_read_register(p, Rd),31)){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),N));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),N));
				
				if(!(arm_read_register(p, Rd))){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),Z));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),Z));
				
				if(CarryFrom(arm_read_register(p, Rn),shifter_operand,0)){ //Carry is on
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),C));
				}else 
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),C));

				if((get_bit(arm_read_register(p,Rn),31) == get_bit(shifter_operand,31)) && (get_bit(arm_read_register(p,Rn),31) != get_bit(arm_read_register(p,Rn)+shifter_operand,31))){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),V));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),V));
				}
			break;

		case(5): //ADC
			result = arm_read_register(p,Rn) + shifter_operand + get_bit(arm_read_cpsr(p),C);
			arm_write_register(p, Rd, result); //Rd = Rn + shifter_operand

			if (S && (arm_read_register(p, Rd) == arm_read_register(p, 15))){
				if(arm_current_mode_has_spsr(p)){
					arm_write_cpsr(p, arm_read_spsr(p));    //CPSR = SPSR
				}
			}else if(S){
				if(get_bit(arm_read_register(p, Rd),31)){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),N));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),N));
				
				if(!(arm_read_register(p, Rd))){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),Z));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),Z));
				
				if(CarryFrom(arm_read_register(p, Rn),shifter_operand,get_bit(arm_read_cpsr(p),C))){ //Carry is on
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),C));
				}else 
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),C));
			
				if((get_bit(arm_read_register(p,Rn),31) == get_bit(shifter_operand,31)) && (get_bit(arm_read_register(p,Rn),31) != get_bit(arm_read_register(p,Rn)+shifter_operand+get_bit(arm_read_cpsr(p),C), 31))){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),V));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),V));
			}	
				
			break;

		case(6): //SBC
			result = arm_read_register(p,Rn) - shifter_operand - ~get_bit(arm_read_cpsr(p),C);
			arm_write_register(p, Rd, result); //Rd = Rn + shifter_operand

			if (S && (arm_read_register(p, Rd) == arm_read_register(p, 15))){
				if(arm_current_mode_has_spsr(p)){
					arm_write_cpsr(p, arm_read_spsr(p));    //CPSR = SPSR
				}
			}else if(S){
				if(get_bit(arm_read_register(p, Rd),31)){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),N));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),N));
				
				if(!(arm_read_register(p, Rd))){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),Z));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),Z));
				
				if(CarryFrom(arm_read_register(p, Rn),shifter_operand,~get_bit(arm_read_cpsr(p),C))){ //Carry is on
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),C));
				}else 
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),C));

				if((get_bit(arm_read_register(p,Rn),31) != get_bit(shifter_operand,31)) && (get_bit(shifter_operand,31) == get_bit(arm_read_register(p,Rn)-(shifter_operand + ~get_bit(arm_read_cpsr(p),C)), 31))){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),V));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),V));
			}					
			break;

		case(7): //RSC
			result = shifter_operand - arm_read_register(p, Rn) - ~get_bit(arm_read_cpsr(p),C);
			arm_write_register(p, Rd, result);
			if (S && (arm_read_register(p, Rd) == arm_read_register(p, 15))){ //if S == 1 and Rd == R15
				if(arm_current_mode_has_spsr(p)){
					arm_write_cpsr(p, arm_read_spsr(p));    //CPSR = SPSR
				}
			}else if(S){
				if(get_bit(arm_read_register(p, Rd),31)){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),N));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),N));
				
				if(!(arm_read_register(p, Rd))){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),Z));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),Z));

				if(BorrowFrom(shifter_operand,arm_read_register(p, Rn),~get_bit(arm_read_cpsr(p),C))){
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),C));
				}else
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),C));

				if((get_bit(shifter_operand,31) != get_bit(arm_read_register(p, Rn),31)) && (get_bit(arm_read_register(p, Rn),31) == get_bit(shifter_operand-(arm_read_register(p, Rn) + ~get_bit(arm_read_cpsr(p),C)), 31))){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),V));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),V));	
				
			}
			break;
	
		case(8): //TST
			alu_out = arm_read_register(p, Rn) & shifter_operand;
			if(get_bit(alu_out,31)){
				arm_write_cpsr(p, set_bit(arm_read_cpsr(p),N));
			}else
				arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),N));
			
			if(!alu_out){
				arm_write_cpsr(p, set_bit(arm_read_cpsr(p),Z));
			}else
				arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),Z));
		
			if(shifter_carryout){
				arm_write_cpsr(p, set_bit(arm_read_cpsr(p),C));
			}else
				arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),C));
			break;

		case(9): //TEQ
			alu_out = arm_read_register(p, Rn) ^ shifter_operand;
			if(get_bit(alu_out,31)){
				arm_write_cpsr(p, set_bit(arm_read_cpsr(p),N));
			}else
				arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),N));
			
			if(!alu_out){
				arm_write_cpsr(p, set_bit(arm_read_cpsr(p),Z));
			}else
				arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),Z));
		
			if(shifter_carryout){
				arm_write_cpsr(p, set_bit(arm_read_cpsr(p),C));
			}else
				arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),C));
			break;

		case(10): //CMP
			alu_out = arm_read_register(p, Rn) - shifter_operand;
			if(get_bit(alu_out,31)){
				arm_write_cpsr(p, set_bit(arm_read_cpsr(p),N));
			}else
				arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),N));

			if(!alu_out){
				arm_write_cpsr(p, set_bit(arm_read_cpsr(p),Z));
			}else
				arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),Z));

			if(BorrowFrom(arm_read_register(p, Rn),shifter_operand,0)){
				arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),C));
			}else
				arm_write_cpsr(p, set_bit(arm_read_cpsr(p),C));

			if((get_bit(arm_read_register(p, Rn),31) != get_bit(shifter_operand,31)) && (get_bit(shifter_operand,31) == get_bit(arm_read_register(p, Rn)-shifter_operand,31))){
				arm_write_cpsr(p, set_bit(arm_read_cpsr(p),V));
			}else
				arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),V));
			break;

		case(11): //ORR
			result = arm_read_register(p, Rn) | shifter_operand;
			arm_write_register(p, Rd, result);
			if (S && (arm_read_register(p, Rd) == arm_read_register(p, 15))){ //if S == 1 and Rd == R15
				if(arm_current_mode_has_spsr(p)){
					arm_write_cpsr(p, arm_read_spsr(p));    //CPSR = SPSR
				}
			}else if(S){
				if(get_bit(arm_read_register(p, Rd),31)){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),N));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),N));
				
				if(!(arm_read_register(p, Rd))){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),Z));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),Z));
				
				if(shifter_carryout){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),C));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),C));
			}
			break;

		case(12): //MOV
			arm_write_register(p, Rd, shifter_operand);
			if (S && (arm_read_register(p, Rd) == arm_read_register(p, 15))){ //if S == 1 and Rd == R15
				if(arm_current_mode_has_spsr(p)){
					arm_write_cpsr(p, arm_read_spsr(p));    //CPSR = SPSR
				}
			}else if(S){
				if(get_bit(arm_read_register(p, Rd),31)){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),N));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),N));
				
				if(!(arm_read_register(p, Rd))){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),Z));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),Z));
				
				if(shifter_carryout){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),C));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),C));
			}
			break;
			
		case(13): //BIC
			result = arm_read_register(p, Rn) & ~(shifter_operand);
			arm_write_register(p, Rd, result);
			if (S && (arm_read_register(p, Rd) == arm_read_register(p, 15))){ //if S == 1 and Rd == R15
				if(arm_current_mode_has_spsr(p)){
					arm_write_cpsr(p, arm_read_spsr(p));    //CPSR = SPSR
				}
			}else if(S){
				if(get_bit(arm_read_register(p, Rd),31)){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),N));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),N));
				
				if(!(arm_read_register(p, Rd))){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),Z));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),Z));
				
				if(shifter_carryout){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),C));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),C));
			}
			break;

		case(14): //MVN
			arm_write_register(p, Rd, ~(shifter_operand));
			if (S && (arm_read_register(p, Rd) == arm_read_register(p, 15))){ //if S == 1 and Rd == R15
				if(arm_current_mode_has_spsr(p)){
					arm_write_cpsr(p, arm_read_spsr(p));    //CPSR = SPSR
				}
			}else if(S){
				if(get_bit(arm_read_register(p, Rd),31)){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),N));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),N));
				
				if(!(arm_read_register(p, Rd))){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),Z));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),Z));
				
				if(shifter_carryout){
					arm_write_cpsr(p, set_bit(arm_read_cpsr(p),C));
				}else
					arm_write_cpsr(p, clr_bit(arm_read_cpsr(p),C));
			}
			break;

	}
		return UNDEFINED_INSTRUCTION;
}

int arm_data_processing_immediate_msr(arm_core p, uint32_t ins) {
	// Voir page 226-227 

	uint8_t operand;
	uint32_t byte_mask;
	if (get_bit(ins, 25) == 1) {
		operand = ror(get_bits(ins,7,0), (get_bits(ins,11,8)*2));
	} else {
		operand = get_bits(ins, 3, 0);
	}
	if ((operand && UnallocMask) != 0) {
		return -1;
	}

	// Initialisation de la valeur du masque
	uint8_t field_mask = get_bits(ins, 19, 16);
	if (get_bit(field_mask, 0) == 1)
		byte_mask = 0x000000FF;
	else 
		byte_mask = 0x00000000;
	if (get_bit(field_mask, 1) == 1)
		byte_mask |= 0x0000FF00;
	else 
		byte_mask |= 0x00000000;
	if (get_bit(field_mask, 2) == 1)
		byte_mask |= 0x00FF0000;
	else 
		byte_mask |= 0x00000000;
	if (get_bit(field_mask, 3) == 1)
		byte_mask |= 0xFF000000;
	else 
		byte_mask |= 0x00000000;

	uint32_t mask;
	if (get_bit(ins, 22) == 0) {
		if (arm_in_a_privileged_mode(p)) {
			if ((operand && StateMask)!= 0) {
				return -1;
			} else {
				mask = (byte_mask && (UserMask || PrivMask));
			}
		} else {
			mask = byte_mask && UserMask;
		} 
		arm_write_cpsr(p, (arm_read_cpsr(p) && ~(mask)) || (operand && mask));
	} else {
		if (arm_current_mode_has_spsr(p)) {
			mask = byte_mask && (UserMask || PrivMask || StateMask);
			arm_write_spsr(p, (arm_read_spsr(p) && ~(mask)) || (operand && mask));
		} else {
			return -1;
		}
	}
	return 0;
}

int CarryFrom(uint32_t op1, uint32_t op2, int c) {
	int i;
	for (i = 0 ; i < 32 ; i++) {
		c = (c + get_bit(op1, i) + get_bit(op2, i)) >> 1;
	}
	return c;
}

int BorrowFrom(uint32_t op1, uint32_t op2, int c) {
	int i;
	for (i = 0 ; i < 32 ; i++) {
		c = (get_bit(op1, i) < (get_bit(op2, i) + c));
	}
	return c;
}
