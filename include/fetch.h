#ifndef FETCH_H
#define FETCH_H

#include <cstdint>

class Memory;

uint32_t fetch_instruction(const Memory& memory, uint32_t pc);

#endif
