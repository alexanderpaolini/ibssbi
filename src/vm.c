#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "vm.h"

#define VERSION 0

unsigned char MAGIC_NUMBER[] = {0x49, 0x42, 0x53, 0x53, 0x42, 0x49};

void _ibssbi_debug_print(ibssbi_program_t *program)
{
    printf("============PROGRAM=============\n");
    printf("VERSION\t\t\t%8d\n", program->header->version);
    printf("ENTRY POINT\t\t%8d\n", program->header->entry_point);
    printf("INSTRUCTIONS SIZE\t%8d\n", program->header->code_segment_size);
    printf("DATA SIZE\t\t%8d\n", program->header->data_segment_size);

    if (program->header->code_segment_size > 0)
    {
        printf("==========INSTRUCTIONS==========\n");
        for (unsigned int i = 0; i < program->header->code_segment_size; i++)
        {
            ibssbi_instruction_t cur_inst = program->code_segment[i];
            for (int i = 31; i >= 0; i--)
                printf("%d", (cur_inst >> i) & 1);
            printf("\n");
        }
    }

    if (program->header->data_segment_size > 0)
    {
        printf("==============DATA==============\n");
        for (unsigned int i = 0; i < program->header->data_segment_size; i++)
        {
            ibssbi_word_t cur_data = program->data_segment[i];
            for (int i = 31; i >= 0; i--)
                printf("%d", (cur_data >> i) & 1);
            printf("\n");
        }
    }
}

void _ibssbi_read_bytes(FILE *f, void *buffer, size_t size)
{
    size_t bytes_read = fread(buffer, 1, size, f);

    if (bytes_read != size)
    {
        if (feof(f))
        {
            fprintf(stderr, "ERROR: REACHED END OF INPUT FILE\n");
        }
        else
        {
            fprintf(stderr, "ERROR: UNKNOWN ERROR READING FILE\n");
        }

        fclose(f);
        exit(EXIT_FAILURE);
    }
}

ibssbi_opcode_types_t _ibssbi_get_opcode_type(ibssbi_instruction_t instruction)
{
    uint8_t type = GET_OPCODE(instruction) & TYPE_MASK;

    switch (type)
    {
    case TYPE_R:
        return TYPE_R;
    case TYPE_I:
        return TYPE_I;
    case TYPE_J:
        return TYPE_J;
    case TYPE_SYS:
        return TYPE_SYS;
    default:
        fprintf(stderr, "ERROR: UNRECOGNIZED OPCODE TYPE");
        fprintf(stderr, "\tTHIS SHOULDN'T BE POSSIBLE");
        exit(EXIT_FAILURE);
    }
}

ibssbi_program_t *read_program(const char *filename)
{
    ibssbi_program_t *program = malloc(sizeof(ibssbi_program_t));

    program->header = malloc(sizeof(ibssbi_header_t));

    FILE *f = fopen(filename, "rb");

    if (f == NULL)
    {
        fprintf(stderr, "ERROR: COULD NOT OPEN INPUT FILE\n");
        fprintf(stderr, "\tARE YOU SURE THIS FILE EXISTS?\n");
        goto failure;
    }

    unsigned char buffer[sizeof(MAGIC_NUMBER)];

    _ibssbi_read_bytes(f, buffer, sizeof(MAGIC_NUMBER));

    for (unsigned long i = 0; i < sizeof(MAGIC_NUMBER); i++)
    {
        if (buffer[i] != MAGIC_NUMBER[i])
        {
            fprintf(stderr, "ERROR: INPUT FILE MAGIC NUMBER DOES NOT MATCH.\n");
            fprintf(stderr, "\tARE YOU SURE THIS IS AN EXECUTABLE?\n");
            goto failure;
        }
    }

    _ibssbi_read_bytes(f, &program->header->version, sizeof(uint32_t));
    _ibssbi_read_bytes(f, &program->header->entry_point, sizeof(uint32_t));
    _ibssbi_read_bytes(f, &program->header->code_segment_size, sizeof(uint32_t));
    _ibssbi_read_bytes(f, &program->header->data_segment_size, sizeof(uint32_t));

    int code_segment_bytes = sizeof(ibssbi_instruction_t) * program->header->code_segment_size;
    program->code_segment = malloc(code_segment_bytes);
    _ibssbi_read_bytes(f, program->code_segment, code_segment_bytes);

    int data_segment_bytes = sizeof(ibssbi_word_t) * program->header->data_segment_size;
    program->data_segment = malloc(data_segment_bytes);
    _ibssbi_read_bytes(f, program->data_segment, data_segment_bytes);

    program->pc = program->header->entry_point;
    program->sp = 0;
    program->fp = 0;

    fclose(f);

#ifdef DEBUG
    _ibssbi_debug_print(program);
#endif

    return program;

failure:
    fclose(f);
    exit(EXIT_FAILURE);
}

ibssbi_status_code_t interpret_program(ibssbi_program_t *program)
{
    while (program->pc < program->header->code_segment_size)
    {
        ibssbi_instruction_t cur_inst = program->code_segment[program->pc];

        ibssbi_opcode_types_t cur_opcode_type = _ibssbi_get_opcode_type(cur_inst);
        switch (cur_opcode_type)
        {
        case TYPE_R:
            switch (GET_OPCODE(cur_inst))
            {
            case NOP:
                break;
            case ADD:
                program->registers[GET_R_RD(cur_inst)] = program->registers[GET_R_RS(cur_inst)] + program->registers[GET_R_RT(cur_inst)];
                break;
            case SUB:
                program->registers[GET_R_RD(cur_inst)] = program->registers[GET_R_RS(cur_inst)] - program->registers[GET_R_RT(cur_inst)];
                break;
            case MUL:
                program->registers[GET_R_RD(cur_inst)] = program->registers[GET_R_RS(cur_inst)] * program->registers[GET_R_RT(cur_inst)];
                break;
            case DIV:
                program->registers[GET_R_RD(cur_inst)] = program->registers[GET_R_RS(cur_inst)] / program->registers[GET_R_RT(cur_inst)];
                break;
            case MOD:
                program->registers[GET_R_RD(cur_inst)] = program->registers[GET_R_RS(cur_inst)] % program->registers[GET_R_RT(cur_inst)];
                break;
            case AND:
                program->registers[GET_R_RD(cur_inst)] = program->registers[GET_R_RS(cur_inst)] & program->registers[GET_R_RT(cur_inst)];
                break;
            case OR:
                program->registers[GET_R_RD(cur_inst)] = program->registers[GET_R_RS(cur_inst)] | program->registers[GET_R_RT(cur_inst)];
                break;
            case XOR:
                program->registers[GET_R_RD(cur_inst)] = program->registers[GET_R_RS(cur_inst)] ^ program->registers[GET_R_RT(cur_inst)];
                break;
            case NOT:
                program->registers[GET_R_RD(cur_inst)] = !program->registers[GET_R_RS(cur_inst)];
                break;
            case SHL:
                program->registers[GET_R_RD(cur_inst)] = program->registers[GET_R_RS(cur_inst)] << program->registers[GET_R_RT(cur_inst)];
                break;
            case SHR:
                program->registers[GET_R_RD(cur_inst)] = program->registers[GET_R_RS(cur_inst)] >> program->registers[GET_R_RT(cur_inst)];
                break;
            default:
                fprintf(stderr, "ERROR: TYPE R OPERATION NOT IMPLEMENTED\n");
                exit(EXIT_FAILURE);
            }
#ifdef DEBUG
            printf("%d, %d, %d, %d\n", GET_OPCODE(cur_inst), GET_R_RD(cur_inst), GET_R_RS(cur_inst), GET_R_RT(cur_inst));
#endif
            break;
        case TYPE_I:
            switch (GET_OPCODE(cur_inst))
            {
            case ADDI:
                program->registers[GET_R_RD(cur_inst)] = program->registers[GET_R_RS(cur_inst)] + GET_I_IMM(cur_inst);
                break;
            case SUBI:
                program->registers[GET_R_RD(cur_inst)] = program->registers[GET_R_RS(cur_inst)] - GET_I_IMM(cur_inst);
                break;
            default:
                fprintf(stderr, "ERROR: TYPE I OPERATION NOT IMPLEMENTED\n");
                exit(EXIT_FAILURE);
            }
#ifdef DEBUG
            printf("%d, %d, %d, %d\n", GET_OPCODE(cur_inst), GET_I_RD(cur_inst), GET_I_RS(cur_inst), GET_I_IMM(cur_inst));
#endif
            break;
        case TYPE_J:
#ifdef DEBUG
            printf("%d, %d\n", GET_OPCODE(cur_inst), GET_J_T(cur_inst));
#endif
            switch (GET_OPCODE(cur_inst))
            {
            case JMP:
                program->pc += GET_J_T(cur_inst);
                break;
            default:
                fprintf(stderr, "ERROR: TYPE J OPERATION NOT IMPLEMENTED\n");
                exit(EXIT_FAILURE);
            }
            break;
        case TYPE_SYS:
#ifdef DEBUG
            printf("SYS, %d\n", GET_SYS_I(cur_inst));
#endif
            switch (GET_SYS_I(cur_inst))
            {
            case SYS_HALT:
                return GET_SYS_I(cur_inst);
                break;
            case SYS_PRINT_CHAR:
                putchar(program->registers[REG_ARGUMENT_0]);
                break;
            case SYS_PRINT_INT:
                printf("%d", program->registers[REG_ARGUMENT_0]);
                break;
            case SYS_PRINT_STRING:
                printf("%s", (char *)&program->data_segment[program->registers[REG_ARGUMENT_0]]);
                break;
            // TODO: handle SYS_PRINT_FLOAT
            // TODO: handle SYS_PRINT_DOUBLE
            default:
                fprintf(stderr, "ERROR: UNKNOWN SYSTEM CALL\n");
                exit(EXIT_FAILURE);
            }
            break;
        }

        program->pc++;
    }

    return SUCCESS;
}
