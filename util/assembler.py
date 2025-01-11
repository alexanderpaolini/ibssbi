import sys

VERSION_MAJOR = 0
VERSION_MINOR = 0
VERSION_PATCH = 1

class InvalidArgumentsException(Exception):
    def __init__(self, message="Invalid arguments provided"):
        super().__init__(message)

opcode_dict = {
    "PUSH": 0x01,
    "POP": 0x02,
    "DUP": 0x03,
    "ADD": 0x10,
    "SUB": 0x11,
    "DIV": 0x12,
    "MULT": 0x13,
    "MOD": 0x14,
    "EQ": 0x15,
    "NEQ": 0x16,
    "LT": 0x17,
    "LTE": 0x18,
    "GT": 0x19,
    "GTE": 0x1A,
    "JMP": 0x20,
    "JMP_IF_TRUE": 0x21,
    "JMP_IF_FALSE": 0x22,
    "PRINT": 0x30,
    "PRINT_INT": 0x31,
    "HALT": 0xFF
}

opcodes_with_values = ["PUSH", "JMP", "JMP_IF_TRUE", "JMP_IF_FALSE"]

def read_input_file(file_path):
    with open(file_path, 'r') as file:
        return file.read()

def parse_tokens(content):
    return content.split()

def process_opcode(token, bytecode):
    if token in opcode_dict:
        bytecode.append(opcode_dict[token].to_bytes(1, byteorder='little'))
        return True
    return False

def process_opcodes_with_values(token, tokens, i, bytecode):
    if token in opcodes_with_values:
        if i < len(tokens):
            value = tokens[i]
            try:
                bytecode.append(int(value).to_bytes(4, byteorder='little'))
            except ValueError:
                raise Exception(f"Invalid value '{value}' for opcode '{token}'")
            return True
        else:
            raise Exception(f"Missing value after opcode '{token}'")
    return False

def assemble_bytecode(tokens):
    bytecode = []
    sz = 0
    i = 0
    
    while i < len(tokens):
        token = tokens[i]
        
        if process_opcode(token, bytecode):
            sz += 1
            i += 1
            
            if process_opcodes_with_values(token, tokens, i, bytecode):
                i += 1
                sz += 4
        else:
            raise Exception(f"Unknown opcode '{token}' at position {i}")
    
    return bytecode, sz

def write_bytecode(bytecode, sz, filename):
    with open(filename, 'wb') as file:
        file.write(VERSION_MAJOR.to_bytes(2, byteorder='little'))
        file.write(VERSION_MINOR.to_bytes(2, byteorder='little'))
        file.write(VERSION_PATCH.to_bytes(2, byteorder='little'))
        file.write(sz.to_bytes(4, byteorder='little'))
        file.write(b''.join(bytecode))

def main():
    if len(sys.argv) != 3:
        raise InvalidArgumentsException("Usage: python assembler.py <input_file> <output_file>")
    
    content = read_input_file(sys.argv[1])
    tokens = parse_tokens(content)
    
    bytecode, sz = assemble_bytecode(tokens)
    
    write_bytecode(bytecode, sz, sys.argv[2])

if __name__ == "__main__":
    main()
