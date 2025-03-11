#ifndef VM_H
#define VM_H

#include <stdio.h>
#include <stdint.h>

// #define DEBUG

#define REGISTER_COUNT 64
#define STACK_SIZE 256
#define MEMORY_SIZE 1024

typedef uint32_t ibssbi_word_t;
typedef ibssbi_word_t ibssbi_register_t[REGISTER_COUNT];
typedef ibssbi_word_t ibssbi_stack_t[STACK_SIZE];
typedef ibssbi_word_t ibssbi_memory_t[MEMORY_SIZE];
typedef uint32_t ibssbi_instruction_t;
typedef ibssbi_word_t *ibssbi_stack_ptr_t;
typedef ibssbi_word_t *ibssbi_frame_ptr_t;

typedef struct
{
    uint32_t version;
    uint32_t entry_point;
    uint32_t code_segment_size;
    uint32_t data_segment_size;
} ibssbi_header_t;

typedef struct
{
    ibssbi_header_t *header;
    ibssbi_instruction_t *code_segment;
    ibssbi_word_t *data_segment;

    ibssbi_register_t registers;
    ibssbi_memory_t memory;
    ibssbi_stack_t stack;

    ibssbi_word_t pc;
    ibssbi_stack_ptr_t sp;
    ibssbi_frame_ptr_t fp;
} ibssbi_program_t;

typedef enum ibssbi_register_location
{
    REG_ZERO = 0,

    REG_STACK_POINTER = 1,
    REG_FRAME_POINTER = 2,
    REG_RETURN_ADDRESS = 3,

    REG_ARGUMENT_0 = 4,
    REG_ARGUMENT_1 = 5,
    REG_ARGUMENT_2 = 6,
    REG_ARGUMENT_3 = 7,

    REG_TEMP_0 = 8,
    REG_TEMP_1 = 9,
    REG_TEMP_2 = 10,
    REG_TEMP_3 = 11,
    REG_TEMP_4 = 12,
    REG_TEMP_5 = 13,
    REG_TEMP_6 = 14,
    REG_TEMP_7 = 15,

    REG_RETURN_1 = 16,
    REG_RETURN_2 = 17,

    REG_SAVE
} ibssbi_register_location_t;

// Haelper macros
#define _LSB4 0b1111
#define _LSB6 0b111111
#define _LSB14 0b11111111111111
#define _LSB26 0b11111111111111111111111111

// Instruction destructuring macros
#define GET_OPCODE(inst) ((ibssbi_opcode_t)((inst >> 26) & 0xFF))
#define GET_OPCODE_NUM(opcode) ((uint8_t)(opcode) & _LSB4)

#define GET_R_RD(inst) ((uint8_t)(inst >> 20) & _LSB6)
#define GET_R_RS(inst) ((uint8_t)(inst >> 14) & _LSB6)
#define GET_R_RT(inst) ((uint8_t)(inst >> 8) & _LSB6)

#define GET_I_RD(inst) ((uint8_t)((inst >> 20) & _LSB6))
#define GET_I_RS(inst) ((uint8_t)((inst >> 14) & _LSB6))
#define GET_I_IMM(inst) ((int16_t)(inst & _LSB14))

// #define GET_J_T(inst) ((int32_t)(inst >> 26) & _LSB26)
#define GET_J_T(INST) ((int32_t)((INST) & _LSB26) << 6 >> 6)

#define GET_SYS_I(inst) ((uint32_t)(inst) & _LSB26)

typedef enum ibssbi_opcode_types
{
    TYPE_R = 0b000000,  // R-type (MSBs = 00)
    TYPE_I = 0b100000,  // I-type (MSBs = 01)
    TYPE_J = 0b010000,  // J-type (MSBs = 10)
    TYPE_SYS = 0b110000 // System / Halt codes (e.g., SYS, HALT) (MSBs = 11)
} ibssbi_opcode_types_t;

#define TYPE_MASK 0b110000

// Generator macros for each of the types
#define _TYPE_R(number) (TYPE_R | (number) & _LSB4)
#define _TYPE_I(number) (TYPE_I | ((number) & _LSB4))
#define _TYPE_J(number) (TYPE_J | ((number) & _LSB4))
#define _TYPE_SYS(number) (TYPE_SYS | ((number) & _LSB4))

typedef enum ibssbi_opcode
{
    NOP = _TYPE_R(0), // No operation

    ADD = _TYPE_R(1),  // Addition: DEST = SRC1 + SRC2
    SUB = _TYPE_R(2),  // Subtraction: DEST = SRC1 - SRC2
    MUL = _TYPE_R(3),  // Multiplication: DEST = SRC1 * SRC2
    DIV = _TYPE_R(4),  // Division: DEST = SRC1 / SRC2
    MOD = _TYPE_R(5),  // Modulo: DEST = SRC1 % SRC2
    AND = _TYPE_R(6),  // Bitwise AND: DEST = SRC1 & SRC2
    OR = _TYPE_R(7),   // Bitwise OR: DEST = SRC1 | SRC2
    XOR = _TYPE_R(8),  // Bitwise XOR: DEST = SRC1 ^ SRC2
    NOT = _TYPE_R(9),  // Bitwise NOT: DEST = ~SRC1 (SRC2 ignored)
    SHL = _TYPE_R(10), // Shift left: DEST = SRC1 << SRC2
    SHR = _TYPE_R(11), // Shift right: DEST = SRC1 >> SRC2

    ADDI = _TYPE_I(1), // Add immediate: DEST = SRC1 + IMM
    SUBI = _TYPE_I(2), // Subtract immediate: DEST = SRC1 - IMM
    MULI = _TYPE_I(3), // Multiply immediate: DEST = SRC1 * IMM
    DIVI = _TYPE_I(4), // Divide immediate: DEST = SRC1 / IMM
    MODI = _TYPE_I(5), // Modulo immediate: DEST = SRC1 % IMM
    ANDI = _TYPE_I(5), // Bitwise AND immediate: DEST = SRC1 & IMM
    ORI = _TYPE_I(6),  // Bitwise OR immediate: DEST = SRC1 | IMM
    XORI = _TYPE_I(7), // Bitwise XOR immediate: DEST = SRC1 ^ IMM
    SHLI = _TYPE_I(8), // Shift left immediate: DEST = SRC1 << IMM
    SHRI = _TYPE_I(9), // Shift right immediate: DEST = SRC1 >> IMM

    LOAD = _TYPE_I(10),  // Load: DEST = MEM[IMM]
    STORE = _TYPE_I(11), // Store: MEM[IMM] = DEST

    PUSH = _TYPE_I(12), // Push to stack: MEM[IMM] = SRC1, SP -= 4
    POP = _TYPE_I(13),  // Pop from stack: SP += 4, DEST = MEM[IMM]

    JMP = _TYPE_J(0),  // Unconditional jump to TARGET
    JEQ = _TYPE_R(12), // Jump if equal: if SRC1 == SRC2 then jump to DEST
    JNE = _TYPE_R(13), // Jump if not equal: if SRC1 != SRC2 then jump to DEST
    JLT = _TYPE_R(14), // Jump if less than: if SRC1 < SRC2 then jump to DEST
    JGT = _TYPE_R(15), // Jump if greater than: if SRC1 > SRC2 then jump to DEST
    JLE = _TYPE_R(16), // Jump if less or equal: if SRC1 <= SRC2 then jump to DEST
    JGE = _TYPE_R(17), // Jump if greater or equal: if SRC1 >= SRC2 then jump to DEST

    CALL = _TYPE_J(1), // Call function at TARGET, save return address in RA
    RET = _TYPE_R(18), // Return from function: jump to address in RA

    HALT = _TYPE_SYS(0), // Halt execution
    SYS = _TYPE_SYS(1)   // System call with code in IMMEDIATE
} ibssbi_opcode_t;

typedef enum ibssbi_system_call
{
    SYS_HALT = 0,
    SYS_PRINT_CHAR = 1,
    SYS_PRINT_INT = 2,
    SYS_PRINT_STRING = 3,
    SYS_PRINT_FLOAT = 4,
    SYS_PRINT_DOUBLE = 5,
} ibssbi_system_call_t;

typedef enum ibssbi_status_code
{
    SUCCESS = 0,
    FAILURE = 1
} ibssbi_status_code_t;

void _ibssbi_debug_print(ibssbi_program_t *program);
void _ibssbi_read_bytes(FILE *f, void *buffer, size_t size);
ibssbi_opcode_types_t _ibssbi_get_opcode_type(ibssbi_instruction_t instruction);

ibssbi_program_t *read_program(const char *filename);
ibssbi_status_code_t interpret_program(ibssbi_program_t *program);

#endif // VM_H
