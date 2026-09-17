#include "decode.h"

namespace {

int32_t sign_extend(uint32_t encoded, unsigned bits){
    if (encoded & (1u << (bits - 1))) {
        encoded |= ~((1u << bits) - 1);
    }
    return static_cast<int32_t>(encoded);
}

}

DecodedInstruction decode_instruction(uint32_t instruction){
    DecodedInstruction decoded = {};
    decoded.opcode = instruction & 0x7F;

    switch (decoded.opcode) {
    case 0x33: // R-type
        decoded.rd = (instruction >> 7) & 0x1F;
        decoded.funct3 = (instruction >> 12) & 0x07;
        decoded.rs1 = (instruction >> 15) & 0x1F;
        decoded.rs2 = (instruction >> 20) & 0x1F;
        decoded.funct7 = (instruction >> 25) & 0x7F;
        break;

    case 0x03: // Loads are I-type
    case 0x13:
    case 0x67:
    case 0x73:
        decoded.rd = (instruction >> 7) & 0x1F;
        decoded.funct3 = (instruction >> 12) & 0x07;
        decoded.rs1 = (instruction >> 15) & 0x1F;
        decoded.immediate = static_cast<int32_t>(instruction) >> 20;
        break;

    case 0x23: { // S-type
        decoded.funct3 = (instruction >> 12) & 0x07;
        decoded.rs1 = (instruction >> 15) & 0x1F;
        decoded.rs2 = (instruction >> 20) & 0x1F;
        uint32_t imm_4_0 = (instruction >> 7) & 0x1F;
        uint32_t imm_11_5 = (instruction >> 25) & 0x7F;
        decoded.immediate = sign_extend((imm_11_5 << 5) | imm_4_0, 12);
        break;
    }

    case 0x63: { // B-type
        decoded.funct3 = (instruction >> 12) & 0x07;
        decoded.rs1 = (instruction >> 15) & 0x1F;
        decoded.rs2 = (instruction >> 20) & 0x1F;
        uint32_t imm_4_1 = (instruction >> 8) & 0x0F;
        uint32_t imm_10_5 = (instruction >> 25) & 0x3F;
        uint32_t imm_11 = (instruction >> 7) & 0x01;
        uint32_t imm_12 = (instruction >> 31) & 0x01;
        uint32_t encoded = (imm_12 << 12) | (imm_11 << 11) |
                           (imm_10_5 << 5) | (imm_4_1 << 1);
        decoded.immediate = sign_extend(encoded, 13);
        break;
    }

    case 0x17: // AUIPC is U-type
    case 0x37: // LUI is U-type
        decoded.rd = (instruction >> 7) & 0x1F;
        // Mask off the top 20 bits in place:
        decoded.immediate = static_cast<int32_t>((instruction >> 12) & 0xFFFFF);   
        break;

    case 0x6F: { // J-type
        decoded.rd = (instruction >> 7) & 0x1F;
        uint32_t imm_19_12 = (instruction >> 12) & 0xFF;
        uint32_t imm11 = (instruction >> 20) & 0x01;
        uint32_t imm_10_1 = (instruction >> 21) & 0x3FF;
        uint32_t imm20 = (instruction >> 31) & 0x01;
        uint32_t encoded = (imm20 << 20) | (imm_19_12 << 12) |
                           (imm11 << 11) | (imm_10_1 << 1);

        decoded.immediate = static_cast<int32_t>(encoded);
        if (encoded & (1u << 20)) {
            decoded.immediate |= ~((1u << 21) - 1);
        }
        break;
    }

    default: break;
    }
    return decoded;
}
