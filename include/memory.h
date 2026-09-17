#ifndef MEMORY_H
#define MEMORY_H

#include <cstddef>
#include <cstdint>
#include <vector>

class Memory {
public:
    explicit Memory(std::size_t size);

    uint8_t read_byte(uint32_t address) const;
    void write_byte(uint32_t address, uint8_t value);

private:
    std::vector<uint8_t> ram;
};

#endif
