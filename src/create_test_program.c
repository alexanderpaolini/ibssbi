#include <stdio.h>
#include <stdlib.h>

#include "vm.h"

int main()
{
    unsigned char MAGIC_NUMBER[] = {0x49, 0x42, 0x53, 0x53, 0x42, 0x49};

    ibssbi_program_t program;

    ibssbi_header_t header;
    program.header = &header;

    header.version = 1;
    header.entry_point = 0;
    header.data_segment_size = 0;

#define R_INST(OP, RD, RS, RT) ( \
    (((OP) & _LSB6) << 26) |     \
    (((RD) & _LSB6) << 20) |     \
    (((RS) & _LSB6) << 14) |     \
    (((RT) & _LSB6) << 8) |      \
    0)

#define I_INST(OP, RD, RS, IMM) ( \
    (((OP) & _LSB6) << 26) |      \
    (((RD) & _LSB6) << 20) |      \
    (((RS) & _LSB6) << 14) |      \
    (((IMM) & _LSB14)))

#define J_INST(OP, TARGET) ( \
    (((OP) & _LSB6) << 26) | \
    ((TARGET) & _LSB26))

#define SYS_INST(OP, TARGET) ( \
    (((OP) & _LSB6) << 26) |   \
    ((TARGET) & _LSB26) |      \
    0)

    ibssbi_instruction_t code_segment[] = {
        I_INST(ADDI, REG_SAVE + 0, REG_ZERO, '\n'),
        R_INST(ADD, REG_SAVE + 1, REG_ZERO, REG_ZERO),
        I_INST(ADDI, REG_SAVE + 1, REG_SAVE + 1, 1),
        R_INST(ADD, REG_ARGUMENT_0, REG_SAVE + 1, REG_ZERO),
        SYS_INST(SYS, SYS_PRINT_INT),
        R_INST(ADD, REG_ARGUMENT_0, REG_SAVE + 0, REG_ZERO),
        SYS_INST(SYS, SYS_PRINT_CHAR),
        J_INST(JMP, -6),
        SYS_INST(SYS, SYS_HALT)};

    header.code_segment_size = sizeof(code_segment) / sizeof(ibssbi_instruction_t);
    program.code_segment = code_segment;

    uint32_t data_segment[] = {};
    header.data_segment_size = sizeof(data_segment) / sizeof(uint32_t);
    program.data_segment = data_segment;

    const char *filename = "test.bin";
    FILE *f = fopen(filename, "wb");

    fwrite(MAGIC_NUMBER, sizeof(MAGIC_NUMBER), 1, f);
    fwrite(&header.version, sizeof(header.version), 1, f);
    fwrite(&header.entry_point, sizeof(header.entry_point), 1, f);
    fwrite(&header.code_segment_size, sizeof(header.code_segment_size), 1, f);
    fwrite(&header.data_segment_size, sizeof(header.data_segment_size), 1, f);
    fwrite(program.code_segment, sizeof(ibssbi_instruction_t), header.code_segment_size, f);
    fwrite(program.data_segment, sizeof(uint32_t), header.data_segment_size, f);

    fclose(f);

    printf("Binary file '%s' created successfully.\n", filename);
}