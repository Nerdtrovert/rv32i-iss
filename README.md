# RV32I CPU Verification & Architecture Lab

An incremental C++ reference-model project for understanding the boundary
between RISC-V instruction encoding and CPU architectural behavior.

The current work focuses on the **Fetch -> Decode** stage: a 32-bit instruction
is reconstructed from byte-addressable memory, then its encoded fields are
made visible through masks, shifts, and debug output. The project is
deliberately small and inspectable so that each architectural assumption can be
compared with the RV32I specification and with instructions emitted by a real
RISC-V assembler.

This is not a cycle-accurate model or a complete CPU simulator. Execution,
architectural state, and a full verification environment are planned next.

## Current Status

**Current stage: Fetch + Decode**

| Area | Status |
| --- | --- |
| Byte-addressable memory | Implemented: 1 KiB `std::vector<uint8_t>` RAM |
| Instruction representation | Implemented: four little-endian bytes form one 32-bit word |
| Instruction fetch | Implemented in `fetch_instruction` |
| Field extraction | Demonstrated with opcode, `rd`, and J-type immediate |
| RV32I format study | Worked through for R, I, S, B, U, and J formats |
| Split immediate reconstruction | Worked through for S, B, and J encodings |
| Assembler comparison | Performed with `riscv64-elf-as` and `objdump` artifacts in `tests/` |
| Debug observability | Implemented through formatted hexadecimal decode output |
| Architectural state | Not implemented |
| Instruction execution | Not implemented |
| Automated regression suite | Not implemented |

The source contains commented extraction examples for the additional formats;
the active runtime path currently fetches the sample J-type instruction and
prints its decoded fields.

## What It Demonstrates

- **ISA encoding and decoding:** mapping specification-defined bit positions to
  concrete C++ masks and shifts.
- **Little-endian memory:** understanding how instruction bytes are laid out in
  memory and reassembled by the fetch stage.
- **Fixed-width integer manipulation:** using `uint8_t`, `uint32_t`, and
  `int32_t` to make data width and conversion behavior explicit.
- **Immediate reconstruction:** combining non-contiguous S-, B-, and J-type
  immediate fragments.
- **Sign extension and alignment:** reasoning about signed immediates and the
  implicit low alignment bit in branch and jump encodings.
- **Architectural observability:** printing the fetched word and decoded fields
  so intermediate results can be inspected deterministically.
- **Verification-oriented development:** comparing known assembler output with
  expected instruction fields before adding execution semantics.

## Architecture

```text
Memory
  |
  v
Fetch
  |
  v
Decode
  |
  v
[Execution - next stage]
```

- **Memory:** stores a 1 KiB byte array and the sample instruction bytes.
- **Fetch:** reads four consecutive bytes at the program counter and combines
  them in little-endian order into a 32-bit instruction.
- **Decode:** isolates fields such as opcode, register indices, function fields,
  and immediate fragments.
- **Execution:** not implemented yet; this is where decoded instructions will
  update architectural state and control flow.

## Instruction Formats

The six base RV32I instruction formats are represented below. The diagrams
match the layouts used while studying the source implementation.

### R-type — register-register ALU operations

```text
31      25 24      20 19      15 14   12 11       7 6       0
+-----------+----------+----------+-------+----------+---------+
|  funct7   |   rs2    |   rs1    | funct3|    rd    | opcode  |
+-----------+----------+----------+-------+----------+---------+
```

R-type instructions encode two source registers and one destination register.
They are used by register-register arithmetic and logical operations.

### I-type — immediate ALU, loads, and other immediate operations

```text
31                 20 19      15 14  12 11      7 6            0
+----------------------+----------+------+---------+--------------+
|      imm[11:0]       |   rs1    |funct3|   rd    |    opcode    |
+----------------------+----------+------+---------+--------------+
```

I-type instructions carry a 12-bit immediate with one source and one
destination register. Loads and immediate arithmetic use this format.

### S-type — stores

```text
31        25 24      20 19      15 14   12 11       7 6       0
+-----------+----------+----------+-------+----------+---------+
| imm[11:5] |   rs2    |   rs1    | funct3| imm[4:0] | opcode  |
+-----------+----------+----------+-------+----------+---------+
```

S-type splits a store offset between the upper and lower portions of the
instruction because the `rd` field is not present.

### B-type — conditional branches

```text
   31    30     25 24    20 19    15 14 12 11       8 7  6      0
+-------+---------+--------+--------+-----+----------+-+--------+
|imm[12]|imm[10:5]|  rs2   |  rs1   |funct| imm[4:1] |i| opcode |
+-------+---------+--------+--------+-----+----------+-+--------+
```

B-type encodes a signed PC-relative branch offset across several bit ranges.
The least-significant offset bit is implicit and represents alignment.

### U-type — upper-immediate operations

```text
31                         12 11        7 6       0
+----------------------------+-----------+---------+
|         imm[31:12]         |    rd     | opcode  |
+----------------------------+-----------+---------+
```

U-type places a 20-bit immediate in the upper portion of the result and is
used by instructions such as `LUI`.

### J-type — unconditional PC-relative jumps

```text
      31     30          21  20  19            12 11        7 6       0
+-----------+-------------+-----+----------------+-----------+---------+
| imm[20]   |  imm[10:1]  |imm11|   imm[19:12]   |    rd     | opcode  |
+-----------+-------------+-----+----------------+-----------+---------+
```

J-type encodes a signed PC-relative jump offset in a split layout. The low
offset bit is implicit because instruction targets are aligned.

## Fetch

`fetch_instruction` reconstructs a 32-bit word by placing each byte at its
corresponding bit offset:

```cpp
instruction |= static_cast<uint32_t>(ram[pc + 0]);
instruction |= static_cast<uint32_t>(ram[pc + 1]) << 8;
instruction |= static_cast<uint32_t>(ram[pc + 2]) << 16;
instruction |= static_cast<uint32_t>(ram[pc + 3]) << 24;
```

For the current sample, RAM contains `ef 02 00 01`, which fetches as
`0x010002ef`.

## Decode

The decoder uses masks and shifts to isolate fields from the fetched word. For
example:

```cpp
uint32_t opcode = instruction & 0x7F;
uint32_t rd = (instruction >> 7) & 0x1F;
uint32_t funct3 = (instruction >> 12) & 0x07;
uint32_t rs1 = (instruction >> 15) & 0x1F;
uint32_t rs2 = (instruction >> 20) & 0x1F;
uint32_t funct7 = (instruction >> 25) & 0x7F;
```

These operations identify encoded fields; they do not execute the instruction.
Instruction semantics belong to the future execution and architectural-state
layers.

## Immediate Reconstruction

S-, B-, and J-type immediates are split because the fixed instruction layout
reuses bit positions for register and function fields. Decoding therefore
requires extracting each fragment and placing it into its architectural
position. Signed values must then be sign-extended to the model's integer
width. Branch and jump offsets also encode alignment: the effective low bit is
implicit rather than stored as a normal instruction field.

For the current J-type sample, the fragments reconstruct an offset of `0x10`,
with `imm[10:1]` shifted left by one before the fields are combined.

## Verification Approach

The current validation workflow uses known instructions emitted by a RISC-V
assembler:

1. Write representative RV32I instructions in `tests/test.s`.
2. Assemble them with `riscv64-elf-as -march=rv32i`.
3. Inspect the machine code with `riscv64-elf-objdump`.
4. Load selected bytes into the model.
5. Compare fetched words and decoded fields against the assembler output.
6. Keep intermediate values visible through deterministic hexadecimal output.

The checked-in assembler output includes R-type ALU instructions, a store,
branches, `LUI`, and `JAL`. Future verification work will add directed tests,
boundary values, negative immediates, illegal instructions, misalignment cases,
regression coverage, and differential comparisons against an established
RISC-V reference simulator. These future checks are not yet implemented as an
automated test suite.

## Build & Run

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -fsanitize=address -O2 main.cpp -o app
./app
```

The executable prints the fetched instruction, opcode, destination register,
and immediate for the active sample.

## Example

The current sample is:

```text
Instruction bytes:  ef 02 00 01
Instruction word:   0x010002ef
```

Decoded fields:

```text
opcode:    0x6f
rd:        5
immediate: 0x10
```

These fields correspond to:

```text
jal x5, 16
```

The project currently decodes this instruction only. It does not yet execute
`JAL`, update a program counter, or write a register.

## Roadmap

```text
Fetch
  -> Decode                 [current]
  -> Register File
  -> Execute
  -> Memory
  -> Control Flow
  -> Full Instruction Loop
  -> Verification
  -> Reference/Differential Testing
  -> Firmware/Bare-Metal Extensions
```

## Engineering Notes / Design Principles

- Prefer correctness and specification alignment over premature optimization.
- Keep architectural state explicit rather than hiding it behind side effects.
- Make fetch, decode, and later execution behavior deterministic and inspectable.
- Build in small milestones with observable intermediate results.
- Validate encodings against the ISA specification and real assembler output.
- Keep decode responsibilities separate from execution semantics.
- Treat boundary conditions and illegal encodings as first-class future tests.

## Why This Project Matters

Instruction decoding is the connection point between an ISA document, machine
code, firmware, and CPU behavior. Building this model develops practical
experience with binary formats, memory ordering, architectural fields, and
debuggable systems software—the same foundations used in instruction tracing,
firmware and silicon enablement, CPU verification, and embedded bring-up.

The value of this project is not a claim of completed processor functionality.
It is a clear, testable path from understanding encoded instructions to
building a reference model and verification workflow.
