#include "execute.h"

void exec_instruction(CPU& cpu, const DecodedInstruction& instr, Memory& memory) {
    (void)memory;
    // Implementation of instruction execution logic goes here
    if(instr.opcode == 0x13){               //opcode OP-IMM
        if(instr.funct3 == 0x00){           //ADDI instruction with funct3 = 0x00 corresponds to the ADDI operation
            uint32_t rs1_val = cpu.read_reg(instr.rs1);
            uint32_t result = rs1_val + instr.immediate;
            cpu.write_reg(instr.rd, result);
        }
    }
    if(instr.opcode == 0x33){
        if(instr.funct3 == 0x00){
            if(instr.funct7== 0x00){
                uint32_t rs1_val = cpu.read_reg(instr.rs1);
                uint32_t rs2_val = cpu.read_reg(instr.rs2);
                uint32_t res = rs1_val + rs2_val;
                cpu.write_reg(instr.rd, res);
            }else if(instr.funct7== 0x20){
                uint32_t rs1_val = cpu.read_reg(instr.rs1);
                uint32_t rs2_val = cpu.read_reg(instr.rs2);
                uint32_t res = rs1_val - rs2_val;
                cpu.write_reg(instr.rd, res);
            }
        }
    }
}