#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdint.h>

#define DEBUG 0

#define STACK_SIZE 256

typedef u_int8_t Byte;
typedef size_t Word;

typedef struct
{
    u_int16_t major;
    u_int16_t minor;
    u_int16_t patch;
    int size;
} Header;

typedef struct
{
    Header *header;
    Byte *instructions;
    FILE *_file;
} Program;

typedef enum
{
    // Stack Minipulation
    PUSH = 0x01,
    POP = 0x02,
    DUP = 0x03,
    SWAP = 0x04,
    ROT = 0x05,
    // Arethmetic
    ADD = 0x10,
    SUB = 0x11,
    DIV = 0x12,
    MULT = 0x13,
    MOD = 0x14,
    POW = 0x15,
    // Comparison
    EQ = 0x20,
    NEQ = 0x21,
    LT = 0x22,
    LTE = 0x23,
    GT = 0x24,
    GTE = 0x25,
    // Bitwise/Logical Operators
    AND = 0x30,
    OR = 0x31,
    XOR = 0x32,
    NOT = 0x33,
    BITWISE_AND = 0x34,
    BITWISE_OR = 0x35,
    BITWISE_XOR = 0x36,
    BITWISE_NOT = 0x37,
    SHIFT_LEFT = 0x38,
    SHIFT_RIGHT = 0x39,
    // Memory Manipulation
    ALLOC = 0x40,
    FREE = 0x41,
    STO = 0x42,
    RET = 0x43,
    // Control Flow
    JMP = 0xE0,
    JMP_IF_TRUE = 0xE1,
    JMP_IF_FALSE = 0xE2,
    // Misc
    PRINT = 0xF0,
    PRINT_INT = 0xF1,
    INPUT = 0xF2,
    INPUT_INT = 0xF3,
    HALT = 0xFF
} __attribute__((packed)) Opcode;

typedef struct
{
    Word stack[STACK_SIZE];
    int sp;
} Stack;

typedef struct
{
    Stack *stack;
    Program *program;
    int pc;
    int running;
} VM;

void push(Stack *s, Word value);
Word pop(Stack *s);

void execute(VM *vm);
int execute_inst(Opcode op, VM *vm);
int execute_simple_inst(Opcode op, VM *vm);
int execute_jump_inst(Opcode op, VM *vm);

FILE *open_program(const char *filename);
Header read_header(FILE *input_file);
Program read_program(Header *header, FILE *input_file);
VM create_vm(Program *program);
void free_vm(VM *vm);

#endif // COMMON_H
