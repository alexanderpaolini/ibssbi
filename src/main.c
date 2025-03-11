#include <stdio.h>
#include <stdlib.h>

#include "vm.h"

int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Expected use: 'ibssbi [filename]'\n");
        return EXIT_FAILURE;
    }

    ibssbi_program_t *prog = read_program(argv[1]);
    ibssbi_status_code_t program_status = interpret_program(prog);

    printf("STATUS: %d\n", program_status);

    return program_status;
}
