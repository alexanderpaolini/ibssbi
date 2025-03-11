# Simple Bytecode VM Specification

This document describes a register-based bytecode virtual machine designed for simplicity and efficiency.

## Overview

The VM is a 32-bit register-based architecture with 64 egisters and support for stack operations. Each instruction is 32 bits wide, providing a balance between expressiveness and simplicity.

## Registers

The VM provides 64 registers (R0-R63), each 32 bits wide:

| Register | Name | Description | Usage |
|----------|------|-------------|-------|
| R0 | ZERO | Hardwired zero | Always contains the value 0 |
| R1 | SP | Stack Pointer | Points to the top of the stack |
| R2 | FP | Frame Pointer | Points to the current call frame |
| R3 | RA | Return Address | Holds the return address for function calls |
| R4-R7 | A0-A3 | Argument Registers | Used for function arguments |
| R8-R15 | T0-T7 | Temporary Registers | Temporary values, not preserved across calls |
| R16-R17 | V0-V1 | Function return values | Set as return values for functions |
| R18-R63 | S0-S45 | Saved Registers | Values preserved across function calls |

## Instruction Formats

All instructions are 32 bits (4 bytes) with the following formats:

### R-Type (Register operations)

```txt
| OPCODE (6 bits) | DEST (6 bits) | SRC1 (6 bits) | SRC2 (6 bits) | FUNC (8 bits) |
```

- OPCODE: Operation category
- DEST: Destination register
- SRC1, SRC2: Source registers
- FUNC: Function code for additional operation specification

### I-Type (Immediate operations)

```txt
| OPCODE (6 bits) | DEST (6 bits) | SRC1 (6 bits) | IMMEDIATE (14 bits) |
```

- IMMEDIATE: 14-bit signed immediate value (-8192 to +8191)

### J-Type (Jump operations)

```txt
| OPCODE (6 bits) | TARGET (26 bits) |
```

- TARGET: Jump target address (absolute or relative)

### SYS-Type (System Calls)

```txt
| OPCODE (6 bits) | TARGET (26 bits) |
```

- TARGET: Jump target address (absolute or relative)

## Opcodes

| Opcode | Name | Format | Description |
|--------|------|--------|-------------|
| 0x00 | NOP | R | No operation |
| 0x01 | ADD | R | Addition: DEST = SRC1 + SRC2 |
| 0x02 | SUB | R | Subtraction: DEST = SRC1 - SRC2 |
| 0x03 | MUL | R | Multiplication: DEST = SRC1 * SRC2 |
| 0x04 | DIV | R | Division: DEST = SRC1 / SRC2 |
| 0x05 | MOD | R | Modulo: DEST = SRC1 % SRC2 |
| 0x06 | AND | R | Bitwise AND: DEST = SRC1 & SRC2 |
| 0x07 | OR | R | Bitwise OR: DEST = SRC1 \| SRC2 |
| 0x08 | XOR | R | Bitwise XOR: DEST = SRC1 ^ SRC2 |
| 0x09 | NOT | R | Bitwise NOT: DEST = ~SRC1 (SRC2 ignored) |
| 0x0A | SHL | R | Shift left: DEST = SRC1 << SRC2 |
| 0x0B | SHR | R | Shift right: DEST = SRC1 >> SRC2 |
| 0x10 | ADDI | I | Add immediate: DEST = SRC1 + IMM |
| 0x11 | SUBI | I | Subtract immediate: DEST = SRC1 - IMM |
| 0x12 | MULI | I | Multiply immediate: DEST = SRC1 * IMM |
| 0x13 | DIVI | I | Divide immediate: DEST = SRC1 / IMM |
| 0x14 | MODI | I | Modulo immediate: DEST = SRC1 % IMM |
| 0x15 | ANDI | I | Bitwise AND immediate: DEST = SRC1 & IMM |
| 0x16 | ORI | I | Bitwise OR immediate: DEST = SRC1 \| IMM |
| 0x17 | XORI | I | Bitwise XOR immediate: DEST = SRC1 ^ IMM |
| 0x18 | SHLI | I | Shift left immediate: DEST = SRC1 << IMM |
| 0x19 | SHRI | I | Shift right immediate: DEST = SRC1 >> IMM |
| 0x20 | LOAD | I | Load: DEST = MEM[IMM] |
| 0x21 | STORE | I | Store: MEM[IMM] = SRC1 |
| 0x22 | PUSH | I | Push to stack: MEM[IMM] = SRC1, SP -= 4 |
| 0x23 | POP | I | Pop from stack: SP += 4, DEST = MEM[IM] |
| 0x30 | JMP | J | Unconditional jump to TARGET |
| 0x31 | JEQ | R | Jump if equal: if SRC1 == SRC2 then jump to DEST |
| 0x32 | JNE | R | Jump if not equal: if SRC1 != SRC2 then jump to DEST |
| 0x33 | JLT | R | Jump if less than: if SRC1 < SRC2 then jump to DEST |
| 0x34 | JGT | R | Jump if greater than: if SRC1 > SRC2 then jump to DEST |
| 0x35 | JLE | R | Jump if less or equal: if SRC1 <= SRC2 then jump to DEST |
| 0x36 | JGE | R | Jump if greater or equal: if SRC1 >= SRC2 then jump to DEST |
| 0x37 | CALL | J | Call function at TARGET, save return address in RA |
| 0x38 | RET | R | Return from function: jump to address in RA |
| 0xFF | HALT | R | Halt execution |
| 0xFE | SYS | I | System call with code in IMMEDIATE |

## Stack Operations

The stack grows downward in memory. The stack pointer (SP, R1) points to the top element of the stack.

- PUSH: Store value at address in SP, then decrement SP by 4
- POP: Increment SP by 4, then load value from address in SP

## Function Call Convention

1. Caller saves temporary registers it wants to preserve
2. Arguments are placed in registers A0-A3 (R4-R7)
3. Additional arguments are pushed onto the stack
4. CALL instruction is executed, saving return address in RA
5. Callee saves FP and adjusts SP/FP as needed
6. Return value is placed in A0 (R4)
7. Callee restores saved registers and FP
8. RET instruction returns to the caller
9. Caller restores any saved temporary registers

## Memory Model

- Byte-addressable memory
- Little-endian byte order
- 32-bit word size
- Instructions are aligned on 4-byte boundaries
- Memory is a linear array of bytes
- No hardware memory protection (implementation-dependent)

## Binary Format

Bytecode files have the following format:

1. Magic number (6 bytes): 0x494253534249 ("IBSSBI")
2. Version (4 bytes): 0x00000001
3. Entry point (4 bytes): Address of first instruction to execute
4. Code segment size (4 bytes): Size of code in bytes
5. Data segment size (4 bytes): Size of data in bytes
6. Code segment: Instructions
7. Data segment: Data accessable as memory, loaded in at runtime
