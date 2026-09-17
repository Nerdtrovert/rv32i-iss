#include "fetch.h"

#include "memory.h"

uint32_t fetch_instruction(const Memory& memory, uint32_t pc){
    uint32_t instruction = 0;
    instruction |= static_cast<uint32_t>(memory.read_byte(pc + 0));
    instruction |= static_cast<uint32_t>(memory.read_byte(pc + 1)) << 8;
    instruction |= static_cast<uint32_t>(memory.read_byte(pc + 2)) << 16;
    instruction |= static_cast<uint32_t>(memory.read_byte(pc + 3)) << 24;
    return instruction;
}
