#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdint.h>

#define DEBUG 0

#define STACK_SIZE 256

typedef u_int8_t Byte;
typedef u_int32_t Value;

typedef struct
{
    short major;
    short minor;
    short patch;
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
    // Arethmetic
    ADD = 0x10,
    SUB = 0x11,
    DIV = 0x12,
    MULT = 0x13,
    MOD = 0x14,
    // Comparison
    EQ = 0x15,
    NEQ = 0x16,
    LT = 0x17,
    LTE = 0x18,
    GT = 0x19,
    GTE = 0x1A,
    // Control Flow
    JMP = 0x20,
    JMP_IF_TRUE = 0x21,
    JMP_IF_FALSE = 0x22,
    // Misc
    PRINT = 0x30,
    PRINT_INT = 0x31,
    HALT = 0xFF
} __attribute__((packed)) Opcode;

typedef struct
{
    Value stack[STACK_SIZE];
    int sp;
} Stack;

typedef struct
{
    Stack *stack;
    Program *program;
    int pc;
    int running;
} VM;

void push(Stack *s, Value value);
Value pop(Stack *s);

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
