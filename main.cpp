#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdint>

using std::cout;
using std::cin;

uint32_t fetch_instruction(const std::vector<uint8_t>& ram, uint32_t pc) {
    uint32_t instruction = 0;
    instruction |= static_cast<uint32_t>(ram[pc+0]);  //tail
    instruction |= static_cast<uint32_t>(ram[pc + 1]) << 8;
    instruction |= static_cast<uint32_t>(ram[pc + 2]) << 16;
    instruction |= static_cast<uint32_t>(ram[pc + 3]) << 24; // head
    return instruction;
}

int main(){
    size_t ram_size = 1024;
    std::vector<uint8_t> ram(ram_size,0);
    ram[0] = 0x13; // Byte 0 (Tail) bits  7:0
    ram[1] = 0x01; // Byte 1 bits 15:8
    ram[2] = 0x50; // Byte 2 bits 23:16
    ram[3] = 0x00; // Byte 3 (Head) bits 31:24
    uint32_t pc = 0;
    uint32_t instruction = fetch_instruction(ram, pc);
/*31                 20 19      15 14  12 11      7 6            0
+----------------------+----------+------+---------+--------------+
|      imm[11:0]       |   rs1    |funct3|   rd    |    opcode    |  I-type
+----------------------+----------+------+---------+--------------+
*/
    uint32_t opcode = (instruction) & 0x7F ;
    uint32_t rd = (instruction >> 7) & 0x1F;
    uint32_t funct32 = (instruction >> 12) & 0x07;
    uint32_t rs1 = (instruction >> 15) & 0x1F;
    int32_t immediate = static_cast<int32_t>(instruction) >> 20;

    cout << "Instruction: 0x" << std::hex << instruction <<"\n";
    cout << "Opcode: 0x" << std::hex << opcode <<"\n";
    cout << "rd: 0x" << std::hex << rd <<"\n";
    cout << "funct3: 0x" << std::hex << funct32 <<"\n";
    cout << "rs1: 0x" << std::hex << rs1 <<"\n";
    cout << "immediate: 0x" << std::hex << immediate <<"\n";

    // if(instruction == 0x00500113) cout << "Instruction: 0x" << std::hex << instruction <<"\n";

}