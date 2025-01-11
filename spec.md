# Incredibly Bad Super Simple Bytecode Interpreter Specification

This simple bytecode virtual machine is stack-based. Operations will be 8 bits. They are optionally followed by a 32 bit value.

**Format**:  
`OP A`  
OP represents the 8-bit opcode.
A represents the input 32-bit integer.

## Operators

### Stack Manipulation Operators

| **Name** | **Opcode** | **A** | **Description**                            |
|----------|------------|-------|--------------------------------------------|
| PUSH     | `0x01`     | `<i8>` | Push integer value to the stack.          |
| POP      | `0x02`     | N/A   | Discard top value from the stack.         |
| DUP      | `0x03`     | N/A   | Push the top of the stack to the top of the stack|

### Arithmetic Operators

| **Name** | **Opcode** | **A** | **Description**                                |
|----------|------------|-------|------------------------------------------------|
| ADD      | `0x10`     | N/A   | Pop 2 values, push their sum to the stack.    |
| SUB      | `0x11`     | N/A   | Pop 2 values, push top minus bottom to the stack. |
| DIV      | `0x12`     | N/A   | Pop 2 values, push top divided by bottom to the stack. |
| MULT     | `0x13`     | N/A   | Pop 2 values, push their product to the stack. |
| MOD      | `0x14`     | N/A   | Pop 2 values, push top mod bottom to the stack. |

### Comparison Operators

| **Name**  | **Opcode** | **A**    | **Description**                                        |
|-----------|------------|----------|--------------------------------------------------------|
| EQ        | `0x15`     | N/A      | Pop 2 values, push `1` if they are equal, otherwise push `0`. |
| NEQ        | `0x16`     | N/A      | Pop 2 values, push `0` if they are equal, otherwise push `1`. |
| LT        | `0x17`     | N/A      | Pop 2 values, push `1` if the first is less than the second, otherwise push `0`. |
| LTE       | `0x18`     | N/A      | Pop 2 values, push `1` if the first is less than or equal to the second, otherwise push `0`. |
| GT        | `0x19`     | N/A      | Pop 2 values, push `1` if the first is greater than the second, otherwise push `0`. |
| GTE       | `0x1A`     | N/A      | Pop 2 values, push `1` if the first is greater than or equal to the second, otherwise push `0`. |

### Control Flow Operators

| **Name**        | **Opcode** | **A** | **Description**                                      |
|-----------------|------------|-------|------------------------------------------------------|
| JMP             | `0x20`     | `<i8>` | Jump to address A (unconditional jump).              |
| JMP_IF_TRUE     | `0x21`     | `<i8>` | Jump to address A if the top value is non-zero.      |
| JMP_IF_FALSE    | `0x22`     | `<i8>` | Jump to address A if the top value is zero.          |

Note: The operand on jump operations is program byte not instruction number. Values are four bytes while opcodes are one byte. Programs must account for this.

### Misc Operators

| **Name**    | **Opcode** | **A**    | **Description**                            |
|-------------|------------|----------|--------------------------------------------|
| HALT        | `0xFF`     | N/A      | Halt execution, status of top value retained. |
| PRINT   | `0x30`     | N/A      | Pop the top value and print it as a character. |
| PRINT_INT   | `0x31`     | N/A      | Pop the top value and print it as an integer. |

### Example Program

```txt
// Step 1: Push the numbers onto the stack
PUSH 1     // 0x01 0x01  // Push 1 to the stack
PUSH 2     // 0x01 0x02  // Push 2 to the stack
PUSH 3     // 0x01 0x03  // Push 3 to the stack
PUSH 4     // 0x01 0x04  // Push 4 to the stack
PUSH 5     // 0x01 0x05  // Push 5 to the stack

// Step 2: Perform addition of all the numbers
ADD       // 0x10 0x00  // Pop two values (5 and 4), add them, push the result (9)
ADD       // 0x10 0x00  // Pop two values (9 and 3), add them, push the result (12)
ADD       // 0x10 0x00  // Pop two values (12 and 2), add them, push the result (14)
ADD       // 0x10 0x00  // Pop two values (14 and 1), add them, push the result (15)

// Step 3: Print the sum
PRINT_INT // 0x30 0x00  // Pop the top (15) and print it.

// Step 4: Halt the program.
PUSH 0    // 0x01 0x00  // Push 0 to the stack.
HALT      // 0xFF 0x00  // Halt the VM, status is 0.
```
