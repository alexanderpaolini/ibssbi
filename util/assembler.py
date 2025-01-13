import sys
import struct

from common import opcode_dict, opcodes_with_values

VERSION_MAJOR = 0
VERSION_MINOR = 0
VERSION_PATCH = 1

size_t = struct.calcsize("P")
u_int8_t = struct.calcsize("B")
u_int16_t = 2 * struct.calcsize("B")
size_int = struct.calcsize("i")

class InvalidArgumentsException(Exception):
    def __init__(self, message="Invalid arguments provided"):
        super().__init__(message)

def read_input_file(file_path):
    with open(file_path, 'r') as file:
        return '\n'.join(map(lambda x: x.split(';')[0], file.read().split('\n')))

def parse_tokens(content):
    return content.split()

def process_opcode(token, bytecode):
    if token in opcode_dict:
        bytecode.append(opcode_dict[token].to_bytes(u_int8_t, byteorder='little'))
        return True
    return False

def process_opcodes_with_values(token, tokens, i, bytecode):
    if token in opcodes_with_values:
        if i < len(tokens):
            value = tokens[i]
            try:
                bytecode.append(int(value).to_bytes(size_t, byteorder='little'))
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
            i += 1
            sz += u_int8_t
            
            if process_opcodes_with_values(token, tokens, i, bytecode):
                i += 1
                sz += size_t
        else:
            raise Exception(f"Unknown opcode '{token}' at position {i}")
    
    return bytecode, sz

def write_bytecode(bytecode, sz, filename):
    with open(filename, 'wb') as file:
        file.write(VERSION_MAJOR.to_bytes(u_int16_t, byteorder='little'))
        file.write(VERSION_MINOR.to_bytes(u_int16_t, byteorder='little'))
        file.write(VERSION_PATCH.to_bytes(u_int16_t, byteorder='little'))
        file.write(sz.to_bytes(size_int, byteorder='little'))
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
