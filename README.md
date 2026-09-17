# RV32I CPU Verification & Architecture Lab

An incremental C++17 reference-model project for learning how RISC-V machine
code becomes architectural state.

The current implementation is deliberately focused on the **Fetch -> Decode**
boundary. It models byte-addressable memory, reconstructs 32-bit
little-endian instructions, and decodes RV32I fields into a
`DecodedInstruction` value. The repository is being developed as a small,
inspectable foundation for later execution modeling and verification—not as a
cycle-accurate design or a complete CPU simulator.

## Current Status

**Current stage: Fetch + Decode**

| Area | Status |
| --- | --- |
| Byte-addressable memory | Implemented by `Memory` with bounds-checked byte access |
| Instruction fetch | Implemented for four little-endian bytes |
| CPU state container | Implemented as a `CPU` struct with 32 registers and `pc` |
| Register access policy | Implemented: `x0` is immutable through `write_reg` |
| Instruction decode | Implemented for R, I, S, B, U, and J opcode classes |
| Field extraction | Implemented for applicable opcode, register, and function fields |
| Immediate reconstruction | Implemented for S-, B-, and J-type encodings |
| Signed immediate handling | Implemented for I-, S-, B-, and J-type paths |
| Decode/execute boundary | Explicit: decode returns data; execution receives decoded data |
| Instruction execution | Not implemented; execution entry point is currently a stub |
| Fetch/decode/execute loop | Not implemented |
| Automated regression tests | Not implemented |

The active program fetches `0x010002ef`, decodes it as a J-type instruction,
and prints its instruction word, opcode, destination register, and immediate.

## Repository Architecture

```text
                         +----------------------+
                         |       src/main.cpp   |
                         |  stimulus + debug IO |
                         +----------+-----------+
                                    |
                                    v
+----------------+       +----------+-----------+       +--------------------+
|  Memory        | ----> |  Fetch               | ----> |  Decode            |
|  include/     |       |  src/fetch.cpp       |       |  src/decode.cpp    |
|  memory.h     |       |  fetch_instruction() |       |  DecodedInstruction|
+-------+--------+       +----------------------+       +---------+----------+
        |                                                         |
        |                                                         v
        |                                             +-----------+----------+
        +-------------------------------------------->|  Execute            |
                                                      |  src/execute.cpp    |
                                                      |  exec_instruction()  |
                                                      |  [stub / next stage] |
                                                      +-----------+----------+
                                                                  |
                                                                  v
                                                      +-----------+----------+
                                                      | CPU architectural   |
                                                      | state: cpu.h        |
                                                      | registers[32], pc   |
                                                      +----------------------+
```

The current data path is:

```text
Memory bytes
    -> fetch_instruction(memory, cpu.pc)
    -> uint32_t instruction
    -> decode_instruction(instruction)
    -> DecodedInstruction
    -> exec_instruction(...)  [not implemented]
```

### Architectural decisions

**Decode and execute are separate interfaces.** `decode_instruction` is a
pure transformation from an encoded `uint32_t` to a `DecodedInstruction`.
Execution is given the decoded representation separately through
`exec_instrcution(CPU&, const DecodedInstruction&, Memory&)`. This keeps
bit-level encoding concerns out of instruction semantics and creates a clear
seam for future unit, directed, and differential testing.

**Architectural state is explicit.** `include/cpu.h` defines `CPU` as a
lightweight struct containing the 32 general-purpose registers and program
counter. `write_reg` protects the RISC-V `x0` invariant, while `read_reg`
provides a bounded register read. Execution will update this state rather than
hide it in decoder-local variables.

**Memory owns byte access.** `Memory` encapsulates the RAM vector and performs
bounds checks in `read_byte` and `write_byte`. Fetch depends on this interface
instead of directly reaching into storage, making invalid accesses observable
and keeping memory policy separate from instruction interpretation.

**Small stages are preferred over a monolithic simulator.** The repository
keeps memory, fetch, decode, execution, and CPU state in separate headers and
translation units. This supports incremental implementation and makes each
stage easier to inspect against the ISA specification.

## What It Demonstrates

- **ISA encoding and decoding:** mapping RV32I field positions to masks and
  shifts.
- **Little-endian memory:** reconstructing an instruction from byte lanes in
  increasing address order.
- **Fixed-width integer manipulation:** using `uint8_t`, `uint32_t`, and
  `int32_t` for explicit data widths.
- **Immediate reconstruction:** combining split S-, B-, and J-type immediate
  fragments.
- **Sign extension and alignment:** preserving signed offsets and accounting
  for implicit alignment bits in branch and jump encodings.
- **Architectural observability:** printing the fetched word and decoded fields
  in a deterministic format.
- **Separation of concerns:** keeping encoding details in decode and reserving
  architectural side effects for execute.
- **Verification-oriented development:** comparing known assembler output with
  expected fields before adding execution semantics.

## Instruction Formats

The decoder covers the six base RV32I instruction formats below.

### R-type — register-register operations

```text
31      25 24      20 19      15 14   12 11       7 6       0
+-----------+----------+----------+-------+----------+---------+
|  funct7   |   rs2    |   rs1    | funct3|    rd    | opcode  |
+-----------+----------+----------+-------+----------+---------+
```

R-type instructions carry two source registers, one destination register, and
the function fields used to select register-register ALU behavior.

### I-type — immediate operations and loads

```text
31                 20 19      15 14  12 11      7 6            0
+----------------------+----------+------+---------+--------------+
|      imm[11:0]       |   rs1    |funct3|   rd    |    opcode    |
+----------------------+----------+------+---------+--------------+
```

I-type instructions use a 12-bit immediate, one source register, and one
destination register. The decoder sign-extends the immediate for the active
I-type opcode classes.

### S-type — stores

```text
31        25 24      20 19      15 14   12 11       7 6       0
+-----------+----------+----------+-------+----------+---------+
| imm[11:5] |   rs2    |   rs1    | funct3| imm[4:0] | opcode  |
+-----------+----------+----------+-------+----------+---------+
```

S-type splits a store offset across two instruction regions because there is no
destination register field.

### B-type — conditional branches

```text
   31    30     25 24    20 19    15 14 12 11       8 7  6      0
+-------+---------+--------+--------+-----+----------+-+--------+
|imm[12]|imm[10:5]|  rs2   |  rs1   |funct| imm[4:1] |i| opcode |
+-------+---------+--------+--------+-----+----------+-+--------+
```

B-type encodes a signed PC-relative offset in non-contiguous fields. The
decoded offset includes the implicit low alignment bit.

### U-type — upper-immediate operations

```text
31                         12 11        7 6       0
+----------------------------+-----------+---------+
|         imm[31:12]         |    rd     | opcode  |
+----------------------------+-----------+---------+
```

U-type carries the upper immediate used by instructions such as `LUI` and
`AUIPC`.

### J-type — unconditional PC-relative jumps

```text
      31     30          21  20  19            12 11        7 6       0
+-----------+-------------+-----+----------------+-----------+---------+
| imm[20]   |  imm[10:1]  |imm11|   imm[19:12]   |    rd     | opcode  |
+-----------+-------------+-----+----------------+-----------+---------+
```

J-type reconstructs a signed jump offset from split fields and an implicit low
alignment bit.

## Fetch

`fetch_instruction` reads four bytes through the `Memory` interface and places
them at bit offsets 0, 8, 16, and 24:

```cpp
instruction |= static_cast<uint32_t>(memory.read_byte(pc + 0));
instruction |= static_cast<uint32_t>(memory.read_byte(pc + 1)) << 8;
instruction |= static_cast<uint32_t>(memory.read_byte(pc + 2)) << 16;
instruction |= static_cast<uint32_t>(memory.read_byte(pc + 3)) << 24;
```

The current stimulus writes `ef 02 00 01` to addresses `0..3`, producing the
fetched instruction `0x010002ef`.

## Decode

`decode_instruction` first extracts the opcode, then selects the format-specific
field layout. Representative field extraction is:

```cpp
decoded.opcode = instruction & 0x7F;
decoded.rd = (instruction >> 7) & 0x1F;
decoded.funct3 = (instruction >> 12) & 0x07;
decoded.rs1 = (instruction >> 15) & 0x1F;
decoded.rs2 = (instruction >> 20) & 0x1F;
decoded.funct7 = (instruction >> 25) & 0x7F;
```

The result is a `DecodedInstruction` containing the fields needed by a future
execution stage. Decode identifies what is encoded; it does not update
registers, memory, or the program counter.

## Immediate Reconstruction

S-, B-, and J-type immediates are split across the instruction because the
encoding reuses bit positions for registers and function fields. The decoder
extracts each fragment, shifts it into its architectural position, combines the
fragments, and sign-extends signed values.

For B- and J-type offsets, the encoded immediate omits bit 0 because valid
instruction targets are aligned. The decoder restores that bit as zero while
reconstructing the effective offset.

## Verification Approach

The repository includes assembler input and disassembly artifacts in `tests/`:

1. Write representative RV32I instructions in `tests/test.s`.
2. Assemble with `riscv64-elf-as -march=rv32i`.
3. Inspect machine code with `riscv64-elf-objdump`.
4. Use known encodings as deterministic decode stimuli.
5. Compare fetched words and decoded fields with the assembler output.
6. Keep intermediate values observable through formatted console output.

The checked-in examples include R-type ALU instructions, a store, branches,
`LUI`, and `JAL`. Automated regression tests, illegal-instruction checks,
boundary-value tests, and differential testing are planned but are not yet
implemented.

## Build & Run

The repository uses the included `Makefile`:

```bash
make
./app
```

The direct compiler configuration used by the Makefile is:

```text
c++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude
```

The current build does not enable AddressSanitizer in the Makefile. It does
use bounds checks in `Memory`; fetch errors propagate through that interface.

## Example

The current sample is:

```text
Instruction bytes:  ef 02 00 01
Instruction word:   0x010002ef
```

The active decode reports:

```text
opcode:    0x6f
rd:        5
immediate: 0x10
```

These fields correspond to:

```text
jal x5, 16
```

The project decodes this instruction only. It does not yet execute `JAL`, write
`x5`, or update `pc`.

## Roadmap

```text
Fetch
  -> Decode                         [current]
  -> CPU architectural state       [container exists; execution use pending]
  -> R-type ALU execution
  -> I-type arithmetic/logical execution
  -> Load/store execution
  -> Branch and jump control flow
  -> Full fetch/decode/execute loop
  -> Instruction tracing
  -> Directed regression verification
  -> Reference/differential testing
  -> Firmware/bare-metal extensions
```

## Engineering Notes

- Prefer correctness and ISA-specification alignment over premature
  optimization.
- Keep architectural state explicit and inspectable.
- Preserve deterministic behavior for repeatable debugging.
- Build one stage at a time with small, observable milestones.
- Validate encodings against both the specification and real assembler output.
- Keep decode free of architectural side effects.
- Keep execution responsible for state updates once it is implemented.
- Treat invalid addresses, illegal encodings, sign boundaries, and alignment
  behavior as first-class verification cases.

## Why This Project Matters

An instruction-set reference model connects ISA documentation, machine code,
firmware, and architectural behavior. This project develops that connection
from the bottom up: byte ordering, field extraction, immediate semantics,
explicit CPU state, and a clean path toward execution and verification.

The result is intentionally not presented as a finished processor. Its value is
the disciplined architecture and progression from an inspectable decoder to a
testable RV32I model suitable for instruction tracing, embedded systems work,
CPU verification, and firmware or silicon enablement workflows.
