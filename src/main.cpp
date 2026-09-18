#include <iomanip>
#include <iostream>

#include "cpu.h"
#include "decode.h"
#include "fetch.h"
#include "memory.h"
#include "execute.h"

int main()
{
    Memory memory(1024);
    //00418133
    memory.write_byte(0, 0x93); // Byte 0 (Tail)
    memory.write_byte(1, 0x01);
    memory.write_byte(2, 0x70);
    memory.write_byte(3, 0x00); // Byte 3 (Head)

    memory.write_byte(4, 0x13); // Byte 4 (Tail)
    memory.write_byte(5, 0x02);
    memory.write_byte(6, 0xd0);
    memory.write_byte(7, 0xff); // Byte 7 (Head)

    memory.write_byte(8, 0x33); // Byte 8 (Tail)
    memory.write_byte(9, 0x81);
    memory.write_byte(10, 0x41);
    memory.write_byte(11, 0x00); // Byte 11 (Head)

    memory.write_byte(12, 0x33); // Byte 12 (Tail)
    memory.write_byte(13, 0x01);
    memory.write_byte(14, 0x32);
    memory.write_byte(15, 0x40); // Byte 15 (Head)

    CPU cpu;

    while(cpu.pc < 16) {
        uint32_t instruction = fetch_instruction(memory, cpu.pc);
        DecodedInstruction decoded = decode_instruction(instruction);
        exec_instruction(cpu, decoded, memory);

        std::cout << "instruction: 0x" << std::hex << std::setw(8)
                  << std::setfill('0') << instruction << "\n";
        std::cout << "opcode: 0x" << std::hex << decoded.opcode << "\n";
        std::cout << "rd: " << std::dec << decoded.rd << "\n";
        std::cout << "immediate: 0x" << std::hex << decoded.immediate << "\n";
        std::cout << "x" << std::dec << decoded.rd << ": 0x"
                  << std::hex << std::setw(8) << std::setfill('0')
                  << cpu.read_reg(decoded.rd) << "\n";

        cpu.pc += 4; // Move to the next instruction
    }

    // if (decoded.opcode != 0x13 || decoded.funct3 != 0x00) {
    //     std::cerr << "unsupported instruction\n";
    //     return 1;
    // }

    return 0;
}