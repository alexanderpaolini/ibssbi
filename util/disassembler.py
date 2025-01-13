import sys
import struct

from common import opcode_dict, opcodes_with_values

# Using it backwards
opcode_dict = {v: k for k, v in opcode_dict.items()}

VERSION_MAJOR = 0
VERSION_MINOR = 0
VERSION_PATCH = 1

size_t = struct.calcsize("P")
u_int8_t = struct.calcsize("B")
u_int16_t = 2 * struct.calcsize("B")
size_int = struct.calcsize("P")

class InvalidVersionException(Exception):
    def __init__(self, version):
        super().__init__(f"Unsupported Executable Version. Expected {VERSION_MAJOR}. Found {version}")

def read_bytes(file_path):
    with open(file_path, 'rb') as file:
        return file.read()

def disassemble(bytecode):
    idx = 0
    output = []
    
    version_major = int.from_bytes(bytecode[idx:idx+u_int16_t], byteorder='little')
    version_minor = int.from_bytes(bytecode[idx+u_int16_t:idx+2*u_int16_t], byteorder='little')
    version_patch = int.from_bytes(bytecode[idx+2*u_int16_t:idx+3*u_int16_t], byteorder='little')
    idx += u_int16_t
    
    if (version_major != VERSION_MAJOR):
        raise InvalidVersionException(version_major)
    
    if (version_minor > VERSION_MINOR):
        print(f"Warning: Executable version does not match. Expected ≤{VERSION_MINOR}. Found {version_minor}.")
    
    size = int.from_bytes(bytecode[idx:idx+size_int], byteorder='little')
    idx += size_int
    
    output.append(f"Version: {version_major}.{version_minor}.{version_patch}")
    output.append(f"Program size: {size}")
    output.append("Disassembled Instructions:")
    
    while idx < len(bytecode):
        opcode = int.from_bytes(bytecode[idx:idx+u_int8_t])
        idx += u_int8_t
        
        if opcode in opcode_dict:
            instruction = opcode_dict[opcode]
            output.append(f"{instruction}")
            
            # Handle opcodes that have associated values
            if instruction in opcodes_with_values:
                val = int.from_bytes(bytecode[idx:idx+size_int], byteorder='little')
                output[-1] += f" {val}"
                idx += size_int
        else:
            output.append(f"Unknown opcode: 0x{opcode:02X}")
    
    return "\n".join(output)

def main():
    if len(sys.argv) != 2:
        print("Usage: python disassembler.py <bytecode_file>")
        sys.exit(1)
    
    bytecode_file = sys.argv[1]
    
    try:
        bytecode = read_bytes(bytecode_file)
        disassembled = disassemble(bytecode)
        print(disassembled)
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    main()
