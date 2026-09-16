cd tests && riscv64-elf-as -march=rv32i -o test.o test.s && riscv64-elf-objdump -d test.o >> test.txt && cd ..


The issue was that the assembler interpreted the raw numbers (8 and -8) as absolute memory addresses rather than relative jumps.
Because it thought you wanted to jump to address 0x8 and a non-existent negative address, it generated incorrect offsets.
To fix it, you must use .+8 and .-8 so the assembler knows to calculate the distance relative to the current instruction.