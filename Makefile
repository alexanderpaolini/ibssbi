# Set the compiler and flags
CC = gcc
CFLAGS = -Wall -g

NAME = ibssbi

# Set the output binary directory and the program name
OUT_DIR = ./out
PROGRAM = $(OUT_DIR)/$(NAME)

# Set the source files
PROGRAM_SRC = ./src/main.c ./src/vm.c
DISASSEMBLER_SRC = ./src/disassembler.c
ASSEMBLER_SRC = ./src/assembler.c

# Create the out directory if it doesn't exist
$(OUT_DIR):
	mkdir -p $(OUT_DIR)

# Rule to compile the program
$(PROGRAM): $(PROGRAM_SRC) | $(OUT_DIR)
	$(CC) $(CFLAGS) -o $(PROGRAM) $(PROGRAM_SRC)

# Rule to clean up the compiled files
clean:
	rm -rf $(OUT_DIR)

# Default target: build the program and disassembler
all: $(PROGRAM)

program: ${PROGRAM}