#ifndef CPU_H
#define CPU_H

#include <cstdint>

struct CPU {
    uint32_t registers[32] = {};
    uint32_t pc = 0;

    void write_reg(uint32_t reg, uint32_t val){
        if(reg!=0 && reg < 32) registers[reg] = val;
    }
    
    uint32_t read_reg(uint32_t reg) const{
        return (reg < 32) ? registers[reg] : 0;
    }
};

#endif
