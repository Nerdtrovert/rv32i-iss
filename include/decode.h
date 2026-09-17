#ifndef DECODE_H
#define DECODE_H

#include <cstdint>

struct DecodedInstruction {
    uint32_t opcode;
    uint32_t rd;
    uint32_t rs1;
    uint32_t rs2;
    uint32_t funct3;
    uint32_t funct7;
    int32_t immediate;
};

DecodedInstruction decode_instruction(uint32_t instruction);

#endif
