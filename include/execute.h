#ifndef EXECUTE_H
#define EXECUTE_H

#include "cpu.h"
#include "memory.h"
#include "decode.h"

void exec_instruction(CPU& cpu, const DecodedInstruction& instr, Memory& memory);
#endif