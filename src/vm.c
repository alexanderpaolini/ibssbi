#include <stdlib.h>
#include <math.h>

#include "vm.h"

/**
 * Explanation of how versioning will work:
 * 
 * A major version change means that a large
 *  part of the program has been rewritten
 *  Example: Changing the header of a file
 *         | Changing the structure of the bytecode
 * 
 * A minor version change means that a feature has been
 *  reworked. This means that code almost definitely will
 *  not work with the new interpreter.
 *  Example: Changing the address of an instruction.
 * 
 * A patch version change means that a small
 *  feature has been added.
 *  Example: Adding a new DEBUG instruction
 * 
 * Note: I dont see myself adding a DEBUG instruction
 */
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

    if (header.minor != VERSION_MINOR)
    {
        fprintf(stderr, "ERROR: EXECUTABLE MINOR VERSION DOES NOT MATCH.\n");
        fprintf(stderr, "\tEXPECTED: %d\n", VERSION_MINOR);
        fprintf(stderr, "\tFOUND: %d\n", header.minor);
        exit(EXIT_FAILURE);
    }

    if (header.minor > VERSION_MINOR)
    {
        fprintf(stderr, "ERROR: EXECUTABLE PATCH VERSION DOES NOT MATCH.\n");
        fprintf(stderr, "\tEXPECTED: <=%d\n", VERSION_PATCH);
        fprintf(stderr, "\tFOUND: %d\n", header.patch);
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

    program.instructions = (Byte *)(malloc(sizeof(Byte) * header->size));
    fread(program.instructions, sizeof(Byte), header->size, input_file);

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

void execute(VM *vm)
{
    while (vm->running)
    {
        vm->pc = execute_inst(vm->program->instructions[vm->pc], vm);
    }
}

Word get_value(int position, Byte *instructions)
{
    // Pointer arithmetic:
    // Cast the value of the bytes starting at position to an int
    return *(Word *)((Byte *)instructions + position);
}

int execute_inst(Opcode op, VM *vm)
{
    int pc = vm->pc;
    Stack *stack = vm->stack;
    Word v1, v2, v3;
    int i;
    switch (op)
    {
    case PUSH:
        push(stack, get_value(pc + sizeof(Byte), vm->program->instructions));
        return pc + sizeof(Byte) + sizeof(Word);
    case POP:
        pop(stack);
        return pc + sizeof(Byte);
    case DUP:
        v1 = pop(stack);
        push(stack, v1);
        push(stack, v1);
        return pc + sizeof(Byte);
    case SWAP:
        v1 = pop(stack);
        v2 = pop(stack);
        push(stack, v1);
        push(stack, v2);
        return pc + sizeof(Byte);
    case ROT:
        v1 = pop(stack); // C
        v2 = pop(stack); // B
        v3 = pop(stack); // A
        push(stack, v2); // ABC -> BCA
        push(stack, v1);
        push(stack, v3);
        return pc + sizeof(Byte);
    case ADD:
    case SUB:
    case DIV:
    case MULT:
    case MOD:
    case POW:
    case EQ:
    case NEQ:
    case LT:
    case LTE:
    case GT:
    case GTE:
    case AND:
    case OR:
    case XOR:
    case BITWISE_AND:
    case BITWISE_OR:
    case BITWISE_XOR:
    case SHIFT_LEFT:
    case SHIFT_RIGHT:
        return execute_simple_inst(op, vm);
    case BITWISE_NOT:
        v1 = pop(stack);
        push(stack, ~v1);
        return pc + sizeof(Byte);
    case NOT:
        v1 = pop(stack);
        push(stack, !v1);
        return pc + sizeof(Byte);
    case ALLOC:
        v1 = get_value(pc + sizeof(Byte), vm->program->instructions);
        Word *mem = malloc(sizeof(Word) * v1);
        if (mem == NULL)
        {
            fprintf(stderr, "ERROR: COULD NOT ALLOCATE MEMORY\n");
            exit(EXIT_FAILURE);
        }
        push(stack, (Word)mem);
        return pc + sizeof(Byte) + sizeof(Word);
    case FREE:
        v1 = pop(stack);
        free((Word *)v1);
        return pc + sizeof(Byte);
    case STO:
        v1 = pop(stack);
        v2 = pop(stack);
        *((Word *)v2) = v1;
        return pc + sizeof(Byte);
    case RET:
        v1 = pop(stack);
        push(stack, *(Word *)v1);
        return pc + sizeof(Byte);
    case JMP:
    case JMP_IF_TRUE:
    case JMP_IF_FALSE:
        return execute_jump_inst(op, vm);
    case PRINT:
        printf("%c", (char)pop(stack));
        return pc + sizeof(Byte);
    case PRINT_INT:
        printf("%lu\n", pop(stack));
        return pc + sizeof(Byte);
    case INPUT:
        push(stack, getc(stdin));
        return pc + sizeof(Byte);
    case INPUT_INT:
        scanf("%d", &i);
        push(stack, i);
        return pc + sizeof(Byte);
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
    Word rhs = pop(vm->stack);
    Word lhs = pop(vm->stack);
    Word result;

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
    case POW:
        result = pow(lhs, rhs);
        break;
    case AND:
        result = lhs && rhs;
        break;
    case OR:
        result = lhs || rhs;
        break;
    case XOR:
        result = (lhs || rhs) && !(lhs && rhs);
        break;
    case BITWISE_AND:
        result = lhs & rhs;
        break;
    case BITWISE_OR:
        result = lhs | rhs;
        break;
    case BITWISE_XOR:
        result = lhs ^ rhs;
        break;
    case SHIFT_LEFT:
        result = lhs << rhs;
        break;
    case SHIFT_RIGHT:
        result = lhs >> rhs;
        break;
    default:
        fprintf(stderr, "ERROR: UNKNOWN SIMPLE INSTRUCTION: 0x%x\n", op);
        exit(EXIT_FAILURE);
        result = 0;
        break;
    }

    push(vm->stack, result);
    return vm->pc + sizeof(Byte);
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
        Word val = get_value(pc + sizeof(Byte), vm->program->instructions);
        return val;
    }
    return pc + sizeof(Byte) + sizeof(Word);
}

void push(Stack *s, Word val)
{
    s->stack[s->sp++] = val;
}

Word pop(Stack *s)
{
    return s->stack[--s->sp];
}

void free_vm(VM *vm)
{
    free(vm->program->instructions);
    free(vm->stack);
}
