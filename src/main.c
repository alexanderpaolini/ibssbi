#include <stdio.h>
#include <stdlib.h>

#include "vm.h"

int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Expected use: 'ibssbi [filename]'\n");
        return 1;
    }

    FILE *input = open_program(argv[1]);
    Header header = read_header(input);

    Program program = read_program(&header, input);
    VM vm = create_vm(&program);

    execute(&vm);

    free_vm(&vm);

    return 0;
}
