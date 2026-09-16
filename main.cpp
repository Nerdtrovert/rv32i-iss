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
    ram[0] = 0xef; // Byte 0 (Tail) bits  7:0
    ram[1] = 0x02; // Byte 1 bits 15:8
    ram[2] = 0x00; // Byte 2 bits 23:16
    ram[3] = 0x01; // Byte 3 (Head) bits 31:24

    // ram[4] = 0xB3;
    // ram[5] = 0x02;
    // ram[6] = 0x73;
    // ram[7] = 0x40;

    // ram[8] = 0xB3;
    // ram[9] = 0x72;
    // ram[10] = 0x73;
    // ram[11] = 0x00;

    // ram[12] = 0xB3;
    // ram[13] = 0x62;
    // ram[14] = 0x73;
    // ram[15] = 0x00;

    // ram[16] = 0xB3;
    // ram[17] = 0x42;
    // ram[18] = 0x73;
    // ram[19] = 0x00;

    uint32_t pc = 0;
    // uint32_t instruction = fetch_instruction(ram, pc);

    // I-type instruction format
/*31                 20 19      15 14  12 11      7 6            0
+----------------------+----------+------+---------+--------------+
|      imm[11:0]       |   rs1    |funct3|   rd    |    opcode    |  I-type
+----------------------+----------+------+---------+--------------+
*/
    //Masking = (1 << bits) - 1
    // uint32_t opcode = (instruction) & 0x7F ;
    // uint32_t rd = (instruction >> 7) & 0x1F;
    // uint32_t funct32 = (instruction >> 12) & 0x07;
    // uint32_t rs1 = (instruction >> 15) & 0x1F;
    // int32_t immediate = static_cast<int32_t>(instruction) >> 20;

    // cout << "Instruction: 0x" << std::hex << instruction <<"\n";
    // cout << "Opcode: 0x" << std::hex << opcode <<"\n";
    // cout << "rd: 0x" << std::hex << rd <<"\n";
    // cout << "funct3: 0x" << std::hex << funct32 <<"\n";
    // cout << "rs1: 0x" << std::hex << rs1 <<"\n";
    // cout << "immediate: " << std::dec << immediate <<"\n";

    //R-type instruction format
/*31      25 24      20 19      15 14   12 11       7 6       0
+-----------+----------+----------+-------+----------+---------+
|  funct7   |   rs2    |   rs1    | funct3|    rd    | opcode  |
+-----------+----------+----------+-------+----------+---------+
*/

    // for(pc =0 ; pc<20; pc+=4){
    //     uint32_t instruction = fetch_instruction(ram, pc);
    //     uint32_t opcode = (instruction) & 0x7F;
    //     uint32_t rd = (instruction >> 7) & 0x1F;
    //     uint32_t funct3 = (instruction >> 12) & 0x07;
    //     uint32_t rs1 = (instruction >> 15) & 0x1F;
    //     uint32_t rs2 = (instruction >> 20 ) & 0x1F;
    //     uint32_t funct7 = (instruction >> 25) & 0x7F;

    //     cout << "instruction: 0x" << std::hex<< instruction <<"\n"; 
    //     cout<< "Opcode: 0x" << std::hex << opcode <<"\n";
    //     cout<<"rd: " << std::hex << rd <<"\n";
    //     cout<<"funct3: 0x" <<std::hex << funct3 <<"\n";
    //     cout << "rs1 : " <<std::hex << rs1 <<"\n";
    //     cout << "rs2 : " <<std::hex << rs2 <<"\n";
    //     cout<<"funct7: 0x" <<std::hex << funct7 <<"\n"; 
    //     cout << "PC: " << std::hex << pc << "\n";
    // }

    // S-type instruction format
/*
31        25 24      20 19      15 14   12 11       7 6       0
+-----------+----------+----------+-------+----------+---------+
| imm[11:5] |   rs2    |   rs1    | funct3| imm[4:0] | opcode  |
+-----------+----------+----------+-------+----------+---------+
*/
    // uint32_t instruction = fetch_instruction(ram,pc);
    // uint32_t opcode = instruction & 0x7F;
    // uint32_t funct3 = (instruction >> 12) & 0x07;
    // uint32_t rs1 = (instruction >> 15) & 0x1F;
    // uint32_t rs2 = (instruction >> 20) & 0x1F;
    // uint32_t imm_7 = (instruction >> 7) & 0x1F;     //imm[4:0]
    // uint32_t imm_25 = (instruction >> 25) & 0x7F;   //imm[11:5]
    
    // uint32_t immediate = (imm_25 << 5) | imm_7;     //combining the immediate, since imm is 5 bits on right, so we leave tht
    //                                                 //space by << 5
    
    // cout << "instruction: 0x" << std::hex<< instruction <<"\n"; 
    // cout<< "Opcode: 0x" << std::hex << opcode <<"\n";
    // cout<<"funct3: 0x" <<std::hex << funct3 <<"\n";
    // cout << "rs1 : " <<std::hex << rs1 <<"\n";
    // cout << "rs2 : " <<std::hex << rs2 <<"\n";
    // cout << "immediate: " << std::dec << immediate <<"\n";

    //B-type instruction format
/*
   31    30     25 24    20 19    15 14 12 11       8 7  6      0
+-------+---------+--------+--------+-----+----------+-+--------+
|imm[12]|imm[10:5]|  rs2   |  rs1   |funct| imm[4:1] |i| opcode |
+-------+---------+--------+--------+-----+----------+-+--------+
*/
    // uint32_t instruction = fetch_instruction(ram,pc);
    // uint32_t opcode = instruction & 0x7F;
    // uint32_t funct3 = (instruction >> 12) & 0x07;
    // uint32_t rs1 = (instruction >> 15) & 0x1F;
    // uint32_t rs2 = (instruction >> 20) & 0x1F;
    // uint32_t imm_4_1 = (instruction >> 8) & 0x0F;
    // uint32_t imm_10_5 = (instruction >> 25) & 0x3F;
    // uint32_t imm_11 = (instruction >> 7) & 0x01;
    // uint32_t imm_12 = (instruction >> 31) & 0x01;
    
    // uint32_t encoded =((imm_12   << 12) |
    //                 (imm_11   << 11) |
    //                 (imm_10_5 << 5)  |
    //                 (imm_4_1  << 1));
    
    // int32_t immediate = static_cast<int32_t>(encoded);
    // if(encoded & (1 << 12)) {   // Check if the sign bit (bit 12) is set
    //     immediate |= ~((1<<13)-1);  // sign extend to 32 bits by setting the upper bits to 1
    // }
    // cout << "instruction: 0x" << std::hex<< instruction <<"\n"; 
    // cout<< "opcode: 0x" << std::hex << opcode <<"\n";
    // cout<<"funct3: 0x" <<std::hex << funct3 <<"\n";
    // cout << "rs1 : " <<std::hex << rs1 <<"\n";
    // cout << "rs2 : " <<std::hex << rs2 <<"\n";
    // cout << "immediate: " << std::dec << immediate <<"\n";

    // U-type instruction format
/*
31                         12 11        7 6       0
+----------------------------+-----------+---------+
|         imm[31:12]         |    rd     | opcode  |
+----------------------------+-----------+---------+
*/
    // uint32_t instruction = fetch_instruction(ram,pc);
    // uint32_t opcode = (instruction) & 0x7F;
    // uint32_t rd = (instruction >> 7) & 0x1F;
    // uint32_t immediate = (instruction >> 12) & 0xFFFFF;

    // cout << "instruction: 0x" << std::hex<< instruction <<"\n"; 
    // cout<< "opcode: 0x" << std::hex << opcode <<"\n";
    // cout << "rd: " << std::dec << rd <<"\n";
    // cout<< "immediate: 0x"<<std::hex << immediate <<"\n";

    // J-type instruction format
/*
      31     30          21  20  19            12 11        7 6       0
+-----------+-------------+-----+----------------+-----------+---------+
| imm[20]   |  imm[10:1]  |imm11|   imm[19:12]   |    rd     | opcode  |
+-----------+-------------+-----+----------------+-----------+---------+
*/
    uint32_t instruction = fetch_instruction(ram,pc);
    uint32_t opcode = (instruction) & 0x7F;
    uint32_t rd = (instruction >> 7) & 0x1F;
    uint32_t imm_19_12 = (instruction >> 12) & 0xFF;
    uint32_t imm11 = (instruction >> 20) & 0x01;
    uint32_t imm_10_1 = (instruction >> 21) & 0x3FF;
    uint32_t imm20 = (instruction >> 31) & 0x01;

    uint32_t encoded = ((imm20 << 20) |
                        (imm_19_12 << 12) |
                        (imm11 << 11) |
                        (imm_10_1 << 1));
    int32_t immediate = static_cast<int32_t>(encoded);


    cout << "instruction: 0x" << std::hex << std::setw(8) << std::setfill('0') << instruction <<"\n";
    cout<< "opcode: 0x" << std::hex << opcode <<"\n";
    cout << "rd: " << std::dec << rd <<"\n";
    cout<< "immediate: 0x"<<std::hex << immediate <<"\n";
}