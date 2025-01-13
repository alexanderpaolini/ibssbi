# Incredibly Bad Super Simple Bytecode Interpreter

**I**ncredibly **B**ad **S**uper **S**imple **B**ytecode **I**nterpreter, or ibssbi, is a bytecode interpreter.

It supports operations such as:

- Push
- Pop
- Add
- Print

Actual specification can be found [here](./spec.md)

Example disassembled program:

```txt
INPUT_INT
INPUT_INT
; START
DUP
PUSH 0
EQ
JMP_IF_TRUE 35
DUP
ROT
SWAP
MOD
JMP 2
; END
POP
PRINT_INT
HALT
```

## Usage

**To Compile**:

Run the following command to build the program:

```bash
make all
```

This will compile the code and output the executable to `./out/ibssbi`.

**To Run**:

Once compiled, you can run the program by providing a filename as an argument:

```bash
ibssbi [filename]
```

Replace `[filename]` with the path to the file you want the program to process.

## Utility Scripts

This project contains two utility scripts designed for working with binary code.

1. **assembler.py**: Converts human-readable code into an executable binary.
2. **disassembler.py**: Converts binary code back into human-readable assembly code.

## 1. **assembler.py**

This script assembles human-readable code into a binary executable file. The input code contained in the file must be valid.

To use the assembler script, run the following command:

```bash
python assembler.py [filename] [out]
```

This will convert the **[filename]** file into a binary executable and save it as **[out]**.

---

## 2. **disassembler.py**

This script disassembles a binary file back into human-readable code. The binary file must have been generated using the assembler script or be in a compatible binary format.

To use the disassembler script, run the following command:

```bash
python disassembler.py [filename]
```

This will convert the **[filename]** binary file into its human-readable form and print it to the terminal.
