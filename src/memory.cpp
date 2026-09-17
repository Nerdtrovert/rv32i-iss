#include "memory.h"

#include <stdexcept>

Memory::Memory(std::size_t size) : ram(size, 0) {}

uint8_t Memory::read_byte(uint32_t address) const{
    if (address >= ram.size()) {
        throw std::out_of_range("memory read out of range");
    }
    return ram[address];
}

void Memory::write_byte(uint32_t address, uint8_t value){
    if (address >= ram.size()) {
        throw std::out_of_range("memory write out of range");
    }
    ram[address] = value;
}
