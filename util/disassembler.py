import sys

VERSION_MAJOR = 0
VERSION_MINOR = 0
VERSION_PATCH = 1

class InvalidVersionException(Exception):
    def __init__(self, version):
        super().__init__(f"Unsupported Executable Version. Expected {VERSION_MAJOR}. Found {version}")

opcode_dict = {
    "PUSH": 0x01,
    "POP": 0x02,
    "DUP": 0x03,
    "SWAP": 0x04,
    "ROT": 0x05,
    "ADD": 0x10,
    "SUB": 0x11,
    "DIV": 0x12,
    "MULT": 0x13,
    "MOD": 0x14,
    "POW": 0x15,
    "EQ": 0x20,
    "NEQ": 0x21,
    "LT": 0x22,
    "LTE": 0x23,
    "GT": 0x24,
    "GTE": 0x25,
    "L_AND": 0x30,
    "L_OR": 0x31,
    "L_XOR": 0x32,
    "L_NOT": 0x33,
    "B_AND": 0x34,
    "B_OR": 0x35,
    "B_XOR": 0x36,
    "B_NOT": 0x37,
    "SHL": 0x38,
    "SHR": 0x39,
    "JMP": 0xE0,
    "JMP_IF_TRUE": 0xE1,
    "JMP_IF_FALSE": 0xE2,
    "PRINT": 0xF0,
    "PRINT_INT": 0xF1,
    "HALT": 0xFF
}

opcodes_with_values = ["PUSH", "JMP", "JMP_IF_TRUE", "JMP_IF_FALSE"]

def read_bytes(file_path):
    with open(file_path, 'rb') as file:
        return file.read()

def disassemble(bytecode):
    idx = 0
    output = []
    
    version_major = int.from_bytes(bytecode[idx:idx+2], byteorder='little')
    version_minor = int.from_bytes(bytecode[idx+2:idx+4], byteorder='little')
    version_patch = int.from_bytes(bytecode[idx+4:idx+6], byteorder='little')
    idx += 6
    
    if (version_major != VERSION_MAJOR):
        raise InvalidVersionException(version_major)
    
    if (version_minor > VERSION_MINOR):
        print(f"Warning: Executable version does not match. Expected ≤{VERSION_MINOR}. Found {version_minor}.")
    
    size = int.from_bytes(bytecode[idx:idx+4], byteorder='little')
    idx += 4
    
    output.append(f"Version: {version_major}.{version_minor}.{version_patch}")
    output.append(f"Program size: {size}")
    output.append("Disassembled Instructions:")
    
    while idx < len(bytecode):
        opcode = bytecode[idx]
        idx += 1
        
        if opcode in opcode_dict:
            instruction = opcode_dict[opcode]
            output.append(f"{instruction}")
            
            # Handle opcodes that have associated values
            if instruction in opcodes_with_values:
                jump_target = int.from_bytes(bytecode[idx:idx+4], byteorder='little')
                output[-1] += f" {jump_target}"
                idx += 4
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
