#include <stdio.h> // for stderr
#include <stdlib.h> // for exit()
#include "types.h"

void decode_instruction(Instruction instruction) {
    
    /* YOUR CODE HERE: COMPLETE THE SWITCH STATEMENTS */
    switch(instruction.opcode) {
        case 0x0: // opcode == 0x0 (SPECIAL) RTYPE STUFF
            switch(instruction.rtype.funct) {
                case 0xc: // funct == 0xc (SYSCALL)
                    printf("syscall\n");
                    break;
			    //Everything below this I wrote ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				case 0x00: // funct == 0x00 sll rd, rt, shamt
                    printf("sll\t$%d,$%d,%d\n",instruction.rtype.rd,instruction.rtype.rt,(uint8_t)instruction.rtype.shamt);
                    break;
				case 0x02: // funct == 02, srl rd, rt, shamt
					printf("srl\t$%d,$%d,%d\n",instruction.rtype.rd,instruction.rtype.rt,(uint8_t)instruction.rtype.shamt);
					break;
				case 0x03: // funct = x03, sra rd, rt, shamt
					printf("sra\t$%d,$%d,%d\n",instruction.rtype.rd,instruction.rtype.rt,(uint8_t)instruction.rtype.shamt);
					break;
				case 0x08: //funct = x08 jr rs
					printf("jr\t$%d\n", instruction.rtype.rs);
					break;
				case 0x09: //funct = x09 jalr rd, rs
					printf("jalr\t$%d,$%d\n", instruction.rtype.rd, instruction.rtype.rs);
					break;
				case 0x10: //funct -- x10 mfhi rd
					printf("mfhi\t$%d\n", instruction.rtype.rd);
					break;
				case 0x12: //funct == x12 mflo rd
					printf("mflo\t$%d\n", instruction.rtype.rd);
					break;
				case 0x18: // funct = x18 mult rs, rt
					printf("mult\t$%d,$%d\n", instruction.rtype.rs, instruction.rtype.rt);
					break;
				case 0x19: //funct = x19 multu rs, rt
					printf("multu\t$%d,$%d\n", instruction.rtype.rs, instruction.rtype.rt);
					break;
				case 0x21: //funct = x21 addu rd, rs, rt
					printf("addu\t$%d,$%d,$%d\n", instruction.rtype.rd, instruction.rtype.rs, instruction.rtype.rt);
					break;
				case 0x23: //funct = x23 subu rd, rs, rt
					printf("subu\t$%d,$%d,$%d\n", instruction.rtype.rd, instruction.rtype.rs, instruction.rtype.rt);
					break;
				case 0x25: //funct = x25 or rd, rs, rt
					printf("or\t$%d,$%d,$%d\n", instruction.rtype.rd, instruction.rtype.rs, instruction.rtype.rt);
					break;
				case 0x26: //funct == x26 xor rd, rs, rt
					printf("xor\t$%d,$%d,$%d\n", instruction.rtype.rd, instruction.rtype.rs, instruction.rtype.rt);
					break;
				case 0x27: //funct == x27 nor rd, rs, rt
					printf("nor\t$%d,$%d,$%d\n", instruction.rtype.rd, instruction.rtype.rs, instruction.rtype.rt);
					break;
				case 0x2a: //funct == x2a slt rd, rs, rt
					printf("slt\t$%d,$%d,$%d\n", instruction.rtype.rd, instruction.rtype.rs, instruction.rtype.rt);
					break;
				case 0x2b: //funct == x2b sltu rd, rs, rt
					printf("sltu\t$%d,$%d,$%d\n", instruction.rtype.rd, instruction.rtype.rs, instruction.rtype.rt);
					break;
				//Everything above this I wrote ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                case 0x24: // funct == 0x24 (AND)
                    printf("and\t$%d,$%d,$%d\n",instruction.rtype.rd,instruction.rtype.rs,instruction.rtype.rt);
                    break;
                default: // undefined funct
                    fprintf(stderr,"%s: illegal function: %08x\n",__FUNCTION__,instruction.bits);
                    exit(-1);
                    break;
            }
            break;
        case 0xd: // opcode == 0xd (ORI) ori rt, rs, imm
            printf("ori\t$%d,$%d,0x%x\n",instruction.itype.rt,instruction.itype.rs,instruction.itype.imm);
            break;
		//I wrote everything below this line ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case 0x04: //opcode == x04 beq rs, rt, offset   I TYPE
			printf("beq\t$%d,$%d,%d\n",instruction.itype.rs,instruction.itype.rt,((int16_t)instruction.itype.imm)*4);
			//the .imm part may be completely wrong, They said to multiply by 4, and that offset should be decimal
			break;
		case 0x05: //opcode == x05 bne rs, rt, offset
			printf("bne\t$%d,$%d,%d\n",instruction.itype.rs,instruction.itype.rt,((int16_t)instruction.itype.imm)*4);
			break;
		case 0x09: //opcode == x09 addiu rt, rs, imm
			printf("addiu\t$%d,$%d,%d\n",instruction.itype.rt,instruction.itype.rs,((int16_t)instruction.itype.imm));
			break;
		case 0x0a: //opcode == x0a slti rt, rs, imm
			printf("slti\t$%d,$%d,%d\n",instruction.itype.rt,instruction.itype.rs,(int16_t)instruction.itype.imm);
			break;
		case 0x0b: //opcode -- x0b sltiu rt, rs, imm
			printf("sltiu\t$%d,$%d,%d\n",instruction.itype.rt,instruction.itype.rs,(int16_t)instruction.itype.imm);
			break;
		case 0x0c: //opcode -- x0c andi rt, rs, imm
			printf("andi\t$%d,$%d,0x%x\n",instruction.itype.rt,instruction.itype.rs,(int16_t)instruction.itype.imm);
			break;
		case 0x0e: //opcode -- x0e xori rt, rs, imm
			printf("xori\t$%d,$%d,0x%x\n",instruction.itype.rt,instruction.itype.rs,(int16_t)instruction.itype.imm);
			break;
		case 0x0f: //opcode -- x0f lui rt, imm
			printf("lui\t$%d,0x%x\n",instruction.itype.rt,instruction.itype.imm);
			break;
		case 0x20: //opcode -- x20 lb rt, offset(rs)
			printf("lb\t$%d,%d($%d)\n",instruction.itype.rt,(int16_t)instruction.itype.imm,instruction.itype.rs);
			break;
		case 0x21: //opcode -- x21 lh rt, offset(rs)
			printf("lh\t$%d,%d($%d)\n",instruction.itype.rt,(int16_t)instruction.itype.imm,instruction.itype.rs);
			break;
		case 0x23: //opcode -- x23 lw rt, offset(rs)
			printf("lw\t$%d,%d($%d)\n",instruction.itype.rt,(int16_t)instruction.itype.imm,instruction.itype.rs);
			break;
		case 0x24: //opcode -- x24 lbu rt, offset(rs)
			printf("lbu\t$%d,%d($%d)\n",instruction.itype.rt,(int16_t)instruction.itype.imm,instruction.itype.rs);
			break;
		case 0x25: //opcode -- x25 lhu rt, offset(rs)
			printf("lhu\t$%d,%d($%d)\n",instruction.itype.rt,(int16_t)instruction.itype.imm,instruction.itype.rs);
			break;
		case 0x28: //opcode -- x28 sb rt, offset(rs)
			printf("sb\t$%d,%d($%d)\n",instruction.itype.rt,(int16_t)instruction.itype.imm,instruction.itype.rs);
			break;
		case 0x29: //opcode -- x29 sh rt, offset(rs)
			printf("sh\t$%d,%d($%d)\n",instruction.itype.rt,(int16_t)instruction.itype.imm,instruction.itype.rs);
			break;
		case 0x2b: //opcode -- x2b sw rt, offset(rs)
			printf("sw\t$%d,%d($%d)\n",instruction.itype.rt,(int16_t)instruction.itype.imm,instruction.itype.rs);
			break;
		//I wrote everything above this line ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case 0x2: // opcode == 0x2 (J)
            printf("j\t0x%x\n",instruction.jtype.addr*4);
            break;
		case 0x03: //opcode == 0x03 jal address ~~~~~~~~~~~~I DID THIS ~~~~~~~~~~~~~~~
			printf("jal\t0x%x\n", instruction.jtype.addr*4);
			break;
        default: // undefined opcode
            fprintf(stderr,"%s: illegal instruction: %08x\n",__FUNCTION__,instruction.bits);
            exit(-1);
            break;
    }
}
