#include <stdio.h> // for stderr
#include <stdlib.h> // for exit()
#include "mips.h" // for execute_syscall()
#include "types.h"

void execute_instruction(Instruction instruction,Processor* processor,Byte *memory) {
    
    /* YOUR CODE HERE: COMPLETE THE SWITCH STATEMENTS */
#define next processor->PC += 4 //because I got annoyed as fuck of writing that statement
    switch(instruction.opcode) {
        case 0x0: // opcode == 0x0(SPECIAL)
            switch(instruction.rtype.funct) {
				// -------------------- BELOW IS ME 0x00 through 0x0c (sll, srl, sra, jr, jalr) ----------------------
				case 0x00: // funct == 0x00 sll rd, rt, shamt ---- R[rd] <- R[rt] <<shamt
					processor->R[instruction.rtype.rd] = (processor->R[instruction.rtype.rt]) << instruction.rtype.shamt;
					next;
					break;
				case 0x02: // funct == 0x02    srl rd, rt, shamt     R[rd] <- R[rt] >> shamt
					processor->R[instruction.rtype.rd] = (processor->R[instruction.rtype.rt]) >> instruction.rtype.shamt;
					next;
					break;
				case 0x03: // funct == 0x03    sra rd, rt, shamt     R[rd] <- (signed)R[rt] >> shamt
					processor->R[instruction.rtype.rd] = (Word)((sWord)(processor->R[instruction.rtype.rt]) >> instruction.rtype.shamt);
					next;
					break;
				case 0x08: // funct == 0x08    jr rs          PC <- R[rs]
					processor->PC = processor->R[instruction.rtype.rs];
					break;
				case 0x09: // jalr rd, rs       tmp <- PC + 4 THEN: PC <- R[rs] THEN: R[rd] <-tmp
					{Word tmp = (processor->PC) + 4;
					processor->PC = processor->R[instruction.rtype.rs];
					processor->R[instruction.rtype.rs] = tmp;}
					break;
				//--------------------- ABOVE IS ME -------------------------------------------
                case 0xc: // funct == 0xc (SYSCALL)
                    execute_syscall(processor);
                    processor->PC += 4;
                    break;
				// ------------------ BELOW IS ME (mflo, mult, multu, addu, subu) ---------------------------
				case 0x10: // mfhi rd        R[rd] <-RHI
					processor->R[instruction.rtype.rd] = processor->RHI;
					next;
					break;
				case 0x12: // mflo rd        R[rd] <-RLO
					processor->R[instruction.rtype.rd] = processor->RLO;
					next;
					break;
				case 0x18: //mult rs, rt     tmp <- (signed)R[rs] * (signed)R[rt] THEN: RLO <- {lower 32 bits of tmp} THEN: RHI <- {upper 32 bits of tmp}
					{sDouble tmp = ((sWord)(processor->R[instruction.rtype.rs])) * ((sWord)(processor->R[instruction.rtype.rs]));
					processor->RLO = (Word)tmp; //lower 32 bits of tmp. Probably unnecessary cast because RLO is predefined as an unsigned Word (int32u or whatever)
					processor->RHI = (Word)(tmp >> 32); //upper 32 bits of tmp. Word is probably unnecesary casting but screw it
					next;}
					break;
				case 0x19: //multu rs, rt    tmp <-R[rs] * R[rt] THEN:   RLO <- {lower 32 bits of tmp}  THEN: RHI <- {upper 32 bits of tmp}
					{Double tmp = (processor->R[instruction.rtype.rs]) * (processor->R[instruction.rtype.rt]);
					processor->RLO = (Word)tmp; //lower 32 bits of tmp. Probably unnecessary cast because RLO is predefined as an unsigned Word (int32u or whatever)
					processor->RHI = (Word)(tmp >> 32); //upper 32 bits of tmp. Word is probably unnecesary casting but screw it
					next;}
					break;
				case 0x21: //addu rd, rs, rt   R[rd] <- R[rs] + R[rt] 
					processor->R[instruction.rtype.rd] = processor->R[(instruction.rtype.rs)] + processor->R[(instruction.rtype.rt)];
					next;
					break;
				case 0x23:  //subu rd, rs, rt   R[rd] <- R[rs] - R[rt]
					processor->R[instruction.rtype.rd] = processor->R[(instruction.rtype.rs)] - processor->R[(instruction.rtype.rt)];
					next;
					break;
				// ------------------ ABOVE IS ME ---------------------------------------------
                case 0x24: // funct == 0x24 (AND)
                    processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs] & processor->R[instruction.rtype.rt];
                    processor->PC += 4;
                    break;
				// ----------------- BELOW IS ME ( or, xor, nor, slt, sltu ) -------------------------------------------
				case 0x25: // or rd, rs, rt     R[rd] <- R[rs] | R[rt]
					processor->R[instruction.rtype.rd] = (processor->R[instruction.rtype.rs]) | processor->R[instruction.rtype.rt];
					next;
					break;
				case 0x26: //xor rd, rs, rt      R[rd] <- R[rs] ^ R[rt]
					processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs] ^ processor->R[instruction.rtype.rt];
					next;
					break;
				case 0x27: // nor rd, rs, rt    R[rd] <- ~(R[rs] | R[rt])
					processor->R[instruction.rtype.rd] = ~(processor->R[instruction.rtype.rs] | processor->R[instruction.rtype.rt]);
					next;
					break;
				case 0x2a: // slt rd, rs, rt       R[rd] <- (signed)R[rs] < (signed)R[rt]
					processor->R[instruction.rtype.rd] = ((sWord)processor->R[instruction.rtype.rs]) < ((sWord)processor->R[instruction.rtype.rt]);
					next;
					break;
				case 0x2b: // sltu rd, rs, rt      R[rd] <- R[rs] < R[rt]
					processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs] < processor->R[instruction.rtype.rt];
					next;
					break;
				// ----------------- ABOVE IS ME ( or, xor, nor, slt, sltu ) -----------------------------------------
                default: // undefined funct
                    fprintf(stderr,"%s: pc=%08x,illegal function=%08x\n",__FUNCTION__,processor->PC,instruction.bits);
                    exit(-1);
                    break;
            }
            break;
        case 0x2: // opcode == 0x2 (J) I PUT THIS COMMENT IN : PC <- {(upper 4 bits of PC + 4), address * 4}
            processor->PC = ((processor->PC+4) & 0xf0000000) | (instruction.jtype.addr << 2);
            break;
		// ----------------------------- BELOW WAS ME (jal, beq, bne, addiu, slti, sltiu, andi )---------------------------------
		case 0x3: //opcode == 0x2 jal address   R[31] <- PC + 4 THEN: PC <- {(upper 4 bits of PC + 4), address * 4}
			processor->R[31] = ((processor->PC) + 4);
			processor->PC = ((processor->PC+4) & 0xf0000000) | (instruction.jtype.addr << 2);
			break;
		case 0x4: //beq rs, rt, offset    if (R[rs] == R[rt]) DO:  PC <- PC + 4 + SignExt(offset) * 4
			if (processor->R[instruction.itype.rs] == processor->R[instruction.itype.rt]){
				processor->PC = processor->PC + 4 + (((sWord)(sHalf)(instruction.itype.imm)) << 2); //hopefully sWord signextends (it should)
			} else {
				next;
			}
			break;
		case 0x5: // bne rs, rt, offset   if(R[rs] != R[rt]) DO : PC <- PC + 4 + SignExt(offset) * 4
			if (processor->R[instruction.itype.rs] != processor->R[instruction.itype.rt]){
				processor->PC = processor->PC + 4 + (((sWord)(sHalf)(instruction.itype.imm)) << 2); //hopefully sWord signextends (it should) IT DOESNT LOL GODDAM THAT KILLED ME
			} else { //same thing as beq just != instead of ==
				next;
			}
			break;
		case 0x9: //addiu rt, rs, imm     R[rt] <- R[rs] + SignExt(imm)
			processor->R[instruction.itype.rt] = processor->R[instruction.itype.rs] + (sWord)(sHalf)instruction.itype.imm;
			next;
			break;
		case 0x0a: //slti rt, rs, imm     R[rt] <- (signed)R[rs] < SignExt(imm)
			processor->R[instruction.itype.rt] = (sWord)processor->R[instruction.itype.rs] < (sWord)(sHalf)instruction.itype.imm;
			next;
			break;
		case 0x0b: // sltiu rt, rs, imm    R[rt] <- R[rs] < SignExt(imm)
			processor->R[instruction.itype.rt] = processor->R[instruction.itype.rs] < (sWord)(sHalf)instruction.itype.imm;
			next;
			break;
		case 0x0c: // andi rt, rs, imm     R[rt] <- R[rs] & ZeroExt(imm)
			processor->R[instruction.itype.rt] = processor->R[instruction.itype.rs] & (Word)(Half)instruction.itype.imm;
			next;
			break;
			// -------------------------- ABOVE WAS ME (jal, beq, bne, addiu, slti, sltiu, andi )-------------------------------
        case 0xd: // opcode == 0xd (ORI)
            processor->R[instruction.itype.rt] = processor->R[instruction.itype.rs] | instruction.itype.imm;
            processor->PC += 4;
            break;
		// ------------------------------ BELOW WAS ME (xori, lui, lb, lh, lw, lbu, lhu, sb, sh, sw) -----------------------------
		case 0xe: // xori rt, rs, imm     R[rt] <- R[rs] ^ ZeroExt(imm)
			processor->R[instruction.itype.rt] = processor->R[instruction.itype.rs] ^ (Word)(Half) instruction.itype.imm;
			next;
			break;
		case 0xf: //lui rt, imm      R[rt] <- imm << 16
			processor->R[instruction.itype.rt] = instruction.itype.imm << 16;
			next;
			break;
		case 0x20: //lb rt, offset(rs)    R[rt] <- SignExt(LoadMem(R[rs] + SignExt(offset), byte))
			processor->R[instruction.itype.rt] = (sWord)(sByte)(load(memory,processor->R[instruction.itype.rs] + ((sWord) (sHalf)instruction.itype.imm), LENGTH_BYTE));
			next;
			break;
		case 0x21: // lh rt, offset(rs)   R[rt] <- SignExt(LoadMem(R[rs] + SignExt(offset), half))
			//printf("-------------------  %d\n", (sWord) ((sHalf)instruction.itype.imm));
			processor->R[instruction.itype.rt] = (sWord)(sHalf)(load(memory,processor->R[instruction.itype.rs] + ((sWord) (sHalf)instruction.itype.imm), LENGTH_HALF_WORD));
			next;
			break;
		case 0x23: // lw rt, offset(rs)   R[rt] <- LoadMem(R[rs] + SignExt(offset), word) 
			processor->R[instruction.itype.rt] =  load(memory,processor->R[instruction.itype.rs] + ((sWord) (sHalf)instruction.itype.imm), LENGTH_WORD);
			next;
			break;
		case 0x24: //lbu rt, offset(rs)  R[rt] <- ZeroExt(LoadMem(R[rs] + SignExt(offset), byte))
			processor->R[instruction.itype.rt] = (Word)(Byte)load(memory,processor->R[instruction.itype.rs] + ((sWord) (sHalf)instruction.itype.imm), LENGTH_BYTE);
			next;
			break;
		case 0x25: //lhu rt, offset(rs)  R[rt] <-ZeroExt(LoadMem(R[rs] + SignExt(offset), half))
			processor->R[instruction.itype.rt] = (Word)(Half)load(memory,processor->R[instruction.itype.rs] + ((sWord) (sHalf)instruction.itype.imm), LENGTH_HALF_WORD);
			next;
			break;
		case 0x28: // sb rt, offset(rs)   StoreMem(R[rs] + SignExt(offset), byte, R[rt]))
			store(memory, processor->R[instruction.itype.rs] + (sWord)(sHalf)instruction.itype.imm, LENGTH_BYTE, processor->R[instruction.itype.rt]);
			next;
			break;
		case 0x29: // sh rt, offset(rs)  StoreMem(R[rs] + SignExt(offset), half, R[rt])
			store(memory, processor->R[instruction.itype.rs] + (sWord)(sHalf)instruction.itype.imm, LENGTH_HALF_WORD, processor->R[instruction.itype.rt]);
			next;
			break;
		case 0x2b: // sw   StoreMem(R[rs] + SignExt(offset), word, R[rt]
			store(memory, processor->R[instruction.itype.rs] + (sWord)(sHalf)instruction.itype.imm, LENGTH_WORD, processor->R[instruction.itype.rt]);
			next;
			break;
		// ------------------------------ ABOVE WAS ME (xori, lui, lb, lh, lw, lbu, lhu, sb, sh, sw) -----------------------------
        default: // undefined opcode
            fprintf(stderr,"%s: pc=%08x,illegal instruction: %08x\n",__FUNCTION__,processor->PC,instruction.bits);
            exit(-1);
            break;
    }
}


int check(Address address,Alignment alignment) {

    if (address > MEMORY_SPACE || address == 0){
		return 0;
	} else if (address % alignment != 0){
		return 0;
	}
    return 1;
}

void store(Byte *memory,Address address,Alignment alignment,Word value) {
    if(!check(address,alignment)) {
        fprintf(stderr,"%s: bad write=%08x\n",__FUNCTION__,address);
        exit(-1);
    }
    *(memory + address) = (Byte) value;
	*(memory + address + 1) = (Byte) (value >> 8);
	*(memory + address + 2) = (Byte) (value >> 16);
	*(memory + address + 3) = (Byte) (value >> 24);
    /* YOUR CODE HERE */
	return;
}

Word load(Byte *memory,Address address,Alignment alignment) {
    if(!check(address,alignment)) {
        fprintf(stderr,"%s: bad read=%08x\n",__FUNCTION__,address);
        exit(-1);
    }
    
    /* YOUR CODE HERE */
    if (alignment == LENGTH_BYTE){
		return *(Byte*)(memory+address);
	} else if (alignment == LENGTH_HALF_WORD){
		return *(Half*)(memory+address);
	}
    // incomplete stub to let "simple" execute
    // (only handles size == SIZE_WORD correctly)
    // feel free to delete and implement your own way
    return *(Word*)(memory+address);
}
