#include <stdlib.h>

#include "vm.h"

#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_PATCH 1

VM create_vm(Program *program)
{
    VM vm;
    vm.pc = 0;
    vm.program = program;
    vm.stack = (Stack *)malloc(sizeof(Stack));
    vm.running = 1;
    return vm;
}

Header read_header(FILE *input_file)
{
    Header header;
    fread(&header.major, sizeof(short), 1, input_file);
    fread(&header.minor, sizeof(short), 1, input_file);
    fread(&header.patch, sizeof(short), 1, input_file);
    fread(&header.size, sizeof(int), 1, input_file);

    if (header.major != VERSION_MAJOR)
    {
        fprintf(stderr, "ERROR: EXECUTABLE MAJOR VERSION DOES NOT MATCH\n");
        fprintf(stderr, "\tEXPECTED: %d\n", VERSION_MAJOR);
        fprintf(stderr, "\tFOUND: %d\n", header.minor);
        exit(EXIT_FAILURE);
    }

    if (header.minor > VERSION_MINOR)
    {
        fprintf(stderr, "ERROR: EXECUTABLE MINOR VERSION IS TOO HIGH.\n");
        fprintf(stderr, "\tEXPECTED: %d or lower\n", VERSION_MINOR);
        fprintf(stderr, "\tFOUND: %d\n", header.minor);
        exit(EXIT_FAILURE);
    }

    if (DEBUG)
    {
        printf("== program ==\n");
        printf("VERSION\t%hi.%hi.%hi\n", header.major, header.minor, header.patch);
        printf("SIZE\t%d\n", header.size);
    }

    return header;
}

FILE *open_program(const char *filename)
{
    FILE *input = fopen(filename, "rb");

    if (input == NULL)
    {
        fprintf(stderr, "ERROR: COULD NOT OPEN INPUT FILE\n");
        exit(EXIT_FAILURE);
    }

    return input;
}

Program read_program(Header *header, FILE *input_file)
{
    Program program;
    program.header = header;

    program.instructions = (u_int8_t *)(malloc(sizeof(u_int8_t) * header->size));
    fread(program.instructions, sizeof(u_int8_t), header->size, input_file);

    if (DEBUG)
    {
        printf("PROGRAM\t");
        for (int i = 0; i < header->size; i++)
        {
            printf("0x%02X ", program.instructions[i]);
        }
        printf("\n");
    }

    fclose(input_file);

    return program;
}

int op_has_value(Opcode op)
{
    switch (op)
    {
    case PUSH:
        return 1;
    default:
        return 0;
    }
}

void execute(VM *vm)
{
    while (vm->running)
    {
        vm->pc = execute_inst(vm->program->instructions[vm->pc], vm);
    }
}

Value get_value(int position, Byte *instructions)
{
    // Pointer arithmetic:
    // Cast the value of the bytes starting at position to an int
    return *(Value *)((Byte *)instructions + position);
}

int execute_inst(Opcode op, VM *vm)
{
    int pc = vm->pc;
    Stack *stack = vm->stack;
    int val;
    switch (op)
    {
    case PUSH:
        push(stack, get_value(++pc, vm->program->instructions));
        return pc + sizeof(Value);
    case POP:
        pop(stack);
        return pc + 1;
    case DUP:
        val = pop(stack);
        push(stack, val);
        push(stack, val);
        return pc + 1;
    case ADD:
    case SUB:
    case DIV:
    case MULT:
    case MOD:
    case EQ:
    case NEQ:
    case LT:
    case LTE:
    case GT:
    case GTE:
        return execute_simple_inst(op, vm);
    case JMP:
    case JMP_IF_TRUE:
    case JMP_IF_FALSE:
        return execute_jump_inst(op, vm);
    case PRINT:
        printf("%c", pop(stack));
        return pc + 1;
    case PRINT_INT:
        printf("%d\n", pop(stack));
        return pc + 1;
    case HALT:
        vm->running = 0;
        return 0;
    default:
        fprintf(stderr, "ERROR: UNKNOWN INSTRUCTION 0x%02X\n", op);
        vm->running = 0;
        exit(1);
    }
}

int execute_simple_inst(Opcode op, VM *vm)
{
    int rhs = pop(vm->stack);
    int lhs = pop(vm->stack);
    int result;

    switch (op)
    {
    case ADD:
        result = lhs + rhs;
        break;
    case SUB:
        result = lhs - rhs;
        break;
    case DIV:
        result = lhs / rhs;
        break;
    case MULT:
        result = lhs * rhs;
        break;
    case MOD:
        result = lhs % rhs;
        break;
    case EQ:
        result = lhs == rhs;
        break;
    case NEQ:
        result = lhs != rhs;
        break;
    case LT:
        result = lhs < rhs;
        break;
    case LTE:
        result = lhs <= rhs;
        break;
    case GT:
        result = lhs > rhs;
        break;
    case GTE:
        result = lhs >= rhs;
        break;
    default:
        result = 0;
        break;
    }

    push(vm->stack, result);
    return vm->pc + 1;
}

int execute_jump_inst(Opcode op, VM *vm)
{
    Stack *stack = vm->stack;
    int pc = vm->pc;

    int top = op == JMP ? 0 : pop(stack);
    if (
        op == JMP ||
        (op == JMP_IF_TRUE && top) ||
        (op == JMP_IF_FALSE && !top))
    {
        Value val = get_value(pc + 1, vm->program->instructions);
        return val;
    }
    return pc + 1 + sizeof(Value);
}

void push(Stack *s, Value val)
{
    s->stack[s->sp++] = val;
}

Value pop(Stack *s)
{
    return s->stack[--s->sp];
}

void free_vm(VM *vm)
{
    free(vm->program->instructions);
    free(vm->stack);
}
