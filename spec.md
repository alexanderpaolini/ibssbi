# Incredibly Bad Super Simple Bytecode Interpreter Specification

This simple bytecode virtual machine is stack-based. Operations will be 8 bits, optionally followed by a 32-bit value.

**Format**:  
`OP A`  
`OP` represents the 8-bit opcode.  
`A` represents the input 32-bit integer.

---

## Operators

### Stack Manipulation Operators

| **Name** | **Opcode** | **A**   | **Description**                                      |
|----------|------------|---------|------------------------------------------------------|
| PUSH     | `0x01`     | `<i8>`  | Push integer value to the stack.                     |
| POP      | `0x02`     | N/A     | Discard top value from the stack.                    |
| DUP      | `0x03`     | N/A     | Push the top of the stack to the top of the stack.   |
| SWAP     | `0x04`     | N/A     | Swap the top two elements of the stack.              |
| ROT      | `0x05`     | N/A     | Rotate the top 3 values on the stack: A B C -> B C A |

---

### Arithmetic Operators

| **Name** | **Opcode** | **A**   | **Description**                                      |
|----------|------------|---------|------------------------------------------------------|
| ADD      | `0x10`     | N/A     | Pop 2 values, push their sum to the stack.           |
| SUB      | `0x11`     | N/A     | Pop 2 values, push top minus bottom to the stack.    |
| DIV      | `0x12`     | N/A     | Pop 2 values, push top divided by bottom to the stack. |
| MULT     | `0x13`     | N/A     | Pop 2 values, push their product to the stack.       |
| MOD      | `0x14`     | N/A     | Pop 2 values, push bottom mod top to the stack.      |
| POW      | `0x15`     | N/A     | Pop 2 values, push bottom ** top to the stack.       |

---

### Comparison Operators

| **Name**  | **Opcode** | **A**   | **Description**                                      |
|-----------|------------|---------|------------------------------------------------------|
| EQ        | `0x20`     | N/A     | Pop 2 values, push `1` if they are equal, otherwise push `0`. |
| NEQ       | `0x21`     | N/A     | Pop 2 values, push `0` if they are equal, otherwise push `1`. |
| LT        | `0x22`     | N/A     | Pop 2 values, push `1` if the first is less than the second, otherwise push `0`. |
| LTE       | `0x23`     | N/A     | Pop 2 values, push `1` if the first is less than or equal to the second, otherwise push `0`. |
| GT        | `0x24`     | N/A     | Pop 2 values, push `1` if the first is greater than the second, otherwise push `0`. |
| GTE       | `0x25`     | N/A     | Pop 2 values, push `1` if the first is greater than or equal to the second, otherwise push `0`. |

---

### Bitwise and Logical Operators

| **Name**        | **Opcode** | **A**   | **Description**                                      |
|-----------------|------------|---------|------------------------------------------------------|
| AND             | `0x30`     | N/A     | Pop two values, push `1` if both are non-zero, else push `0`. |
| OR              | `0x31`     | N/A     | Pop two values, push `1` if at least one is non-zero, else push `0`. |
| XOR             | `0x32`     | N/A     | Pop two values, push `1` if exactly one is non-zero, else push `0`. |
| NOT             | `0x33`     | N/A     | Pop one value, push `1` if the value is zero, else push `0`. |
| BITWISE_AND     | `0x34`     | N/A     | Pop two values, perform bitwise AND, and push the result. |
| BITWISE_OR      | `0x35`     | N/A     | Pop two values, perform bitwise OR, and push the result. |
| BITWISE_XOR     | `0x36`     | N/A     | Pop two values, perform bitwise XOR, and push the result. |
| BITWISE_NOT     | `0x37`     | N/A     | Pop one value, perform bitwise NOT (invert bits), and push the result. |
| SHIFT_LEFT      | `0x38`     | N/A     | Pop two values, shift the first value left by the second (bitwise shift), and push the result. |
| SHIFT_RIGHT     | `0x39`     | N/A     | Pop two values, shift the first value right by the second (bitwise shift), and push the result. |

---

### Memory Manipulation Operators

| **Name**    | **Opcode** | **A**    | **Description**                                      |
|-------------|------------|----------|------------------------------------------------------|
| ALLOC       | `0x40`     | `<i8>`   | Allocate `A` bytes and return the address.           |
| FREE        | `0x41`     | `<i8>`   | Free the memory at the address at the top of the stack. |
| STO         | `0x42`     | N/A      | Store the top of the stack to the address below it. Pops both values from the stack. |
| RET         | `0x43`     | N/A      | Pushes the value at the address popped from the stack to the stack. |

---

### Control Flow Operators

| **Name**        | **Opcode** | **A**    | **Description**                                      |
|-----------------|------------|----------|------------------------------------------------------|
| JMP             | `0xE0`     | `<i8>`   | Jump to address `A` (unconditional jump).            |
| JMP_IF_TRUE     | `0xE1`     | `<i8>`   | Jump to address `A` if the top value is non-zero.    |
| JMP_IF_FALSE    | `0xE2`     | `<i8>`   | Jump to address `A` if the top value is zero.        |

> **Note**: The operand on jump operations is the program byte, not the instruction number. Values are four bytes, while opcodes are one byte. Programs must account for this.

---

### IO Operators

| **Name**        | **Opcode** | **A**    | **Description**                                      |
|-----------------|------------|----------|------------------------------------------------------|
| PRINT           | `0xF0`     | N/A      | Pop the top value and print it as a character.       |
| PRINT_INT       | `0xF1`     | N/A      | Pop the top value and print it as an integer.        |
| INPUT           | `0xF3`     | N/A      | Push the input value (character value) to the stack. |
| INPUT_INT       | `0xF2`     | N/A      | Push the input value (int value) to the stack.       |

---

### Misc Operators

| **Name**        | **Opcode** | **A**    | **Description**                                      |
|-----------------|------------|----------|------------------------------------------------------|
| HALT            | `0xFF`     | N/A      | Halt execution, status of top value retained.        |
