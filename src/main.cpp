#include <iomanip>
#include <iostream>

#include "cpu.h"
#include "decode.h"
#include "fetch.h"
#include "memory.h"

int main(){
    Memory memory(1024);
    memory.write_byte(0, 0xef); // Byte 0 (Tail)
    memory.write_byte(1, 0x02);
    memory.write_byte(2, 0x00);
    memory.write_byte(3, 0x01); // Byte 3 (Head)

    CPU cpu;
    uint32_t instruction = fetch_instruction(memory, cpu.pc);
    DecodedInstruction decoded = decode_instruction(instruction);

    std::cout << "instruction: 0x" << std::hex << std::setw(8)
              << std::setfill('0') << instruction << "\n";
    std::cout << "opcode: 0x" << std::hex << decoded.opcode << "\n";
    std::cout << "rd: " << std::dec << decoded.rd << "\n";
    std::cout << "immediate: 0x" << std::hex << decoded.immediate << "\n";
}
