import os
import subprocess

tests = [
    (
        "test_division",
        """
            PUSH 10
            PUSH 2
            DIV
            PRINT_INT
            HALT
        """,
        [5]
    ),
    (
        "test_duplicate",
        """
            PUSH 5
            DUP
            PRINT_INT
            PRINT_INT
            HALT
        """,
        [5, 5]
    ),
    (
        "test_jump", 
        """
            PUSH 9
            JMP 27
            PUSH 0
            PRINT_INT
            HALT
        """,
        [9]
    ),
    (
        "test_gte",
        """
            PUSH 4
            PUSH 5
            GTE
            PRINT_INT

            PUSH 5
            PUSH 5
            GTE
            PRINT_INT

            PUSH 5
            PUSH 4
            GTE
            PRINT_INT

            HALT
        """,
        [0, 1, 1]
    ),
    (
        "test_pop",
        """
            PUSH 8
            PUSH 3
            POP
            PRINT_INT
            HALT
        """,
        [8]
    ),
    (
        "test_modulo",
        """
            PUSH 10
            PUSH 3
            MOD
            PRINT_INT

            PUSH 3
            PUSH 3
            MOD
            PRINT_INT

            HALT
        """,
        [1, 0]
    ),
    (
        "test_lte",
        """
            PUSH 4
            PUSH 5
            LTE
            PRINT_INT

            PUSH 5
            PUSH 5
            LTE
            PRINT_INT

            PUSH 5
            PUSH 4
            LTE
            PRINT_INT

            HALT
        """,
        [1, 1, 0]
    ),
    (
        "test_jmp_if_true",
        """
            PUSH 7
            PUSH 1
            JMP_IF_TRUE 36
            PUSH 3
            PRINT_INT
            HALT
        """,
        [7]
    ),
    (
        "test_jmp_if_false", 
        """
            PUSH 7
            PUSH 0
            JMP_IF_FALSE 36
            PUSH 3
            PRINT_INT
            HALT
        """,
        [7]
    ),
    (
        "test_subtraction",
        """
            PUSH 5
            PUSH 3
            SUB
            PRINT_INT
            
            PUSH 5
            PUSH 5
            SUB
            PRINT_INT

            HALT
        """,
        [2, 0]
    ),
    (
        "test_eq",
        """
            PUSH 4
            PUSH 4
            EQ
            PRINT_INT

            PUSH 4
            PUSH 5
            EQ
            PRINT_INT

            HALT
        """,
        [1, 0]
    ),
    (
        "test_neq",
        """
            PUSH 4
            PUSH 5
            NEQ
            PRINT_INT

            PUSH 5
            PUSH 5
            NEQ
            PRINT_INT

            HALT
        """,
        [1, 0]
    ),
    (
        "test_lt",
        """
            PUSH 4
            PUSH 5
            LT
            PRINT_INT

            PUSH 5
            PUSH 5
            LT
            PRINT_INT

            PUSH 5
            PUSH 4
            LT
            PRINT_INT

            HALT
        """,
        [1, 0, 0]
    ),
    (
        "test_gt",
        """
            PUSH 4
            PUSH 5
            GT
            PRINT_INT

            PUSH 5
            PUSH 5
            GT
            PRINT_INT

            PUSH 5
            PUSH 4
            GT
            PRINT_INT

            HALT
        """,
        [0, 0, 1]
    ),
    (
        "test_multiplication",
        """
            PUSH 4
            PUSH 3
            MULT
            PRINT_INT
            
            HALT
        """,
        [12]
    ),
    (
        "test_addition",
        """
            PUSH 1
            PUSH 2
            ADD
            PRINT_INT
            
            HALT
        """,
        [3]
    ),
    (
        "test_logical_and",
        """
            PUSH 1
            PUSH 1
            L_AND
            PRINT_INT
            
            HALT
        """,
        [1]
    ),
    (
        "test_logical_or",
        """
            PUSH 0
            PUSH 1
            L_OR
            PRINT_INT
            
            HALT
        """,
        [1]
    ),
    (
        "test_logical_xor",
        """
            PUSH 1
            PUSH 0
            L_XOR
            PRINT_INT
            
            HALT
        """,
        [1]
    ),
    (
        "test_logical_not",
        """
            PUSH 0
            L_NOT
            PRINT_INT
            
            PUSH 1
            L_NOT
            PRINT_INT
            
            HALT
        """,
        [1, 0]
    ),
    (
        "test_bitwise_and",
        """
            PUSH 5
            PUSH 3
            B_AND
            PRINT_INT
            
            HALT
        """,
        [1]
    ),
    (
        "test_bitwise_or",
        """
            PUSH 5
            PUSH 3
            B_OR
            PRINT_INT
            
            HALT
        """,
        [7]
    ),
    (
        "test_bitwise_xor",
        """
            PUSH 5
            PUSH 3
            B_XOR
            PRINT_INT
            
            HALT
        """,
        [6]
    ),
    (
        "test_bitwise_not",
        """
            PUSH 5
            B_NOT
            PRINT_INT
            
            HALT
        """,
        [18446744073709551610]
    ),
    (
        "test_power",
        """
            PUSH 2
            PUSH 3
            POW
            PRINT_INT
            
            HALT
        """,
        [8]
    ),
    (
        "test_swap",
        """
            PUSH 1
            PUSH 2
            SWAP
            PRINT_INT
            PRINT_INT
            
            HALT
        """,
        [1, 2]
    ),
    (
        "test_rotate",
        """
            PUSH 1
            PUSH 2
            PUSH 3
            ROT
            PRINT_INT
            PRINT_INT
            PRINT_INT
            
            HALT
        """,
        [1, 3, 2]
    )
]

temp_dir = './temp_tests'
os.makedirs(temp_dir, exist_ok=True)

for test_name, code, expected_output in tests:
    def clean_up():
        os.remove(test_file)
    
    test_file = os.path.join(temp_dir, f"{test_name}.asm")
    with open(test_file, 'w') as f:
        f.write(code)

    assemble_command = f"python3 ../util/assembler.py {test_file} ./out"
    assemble_process = subprocess.run(assemble_command, shell=True, capture_output=True)
    
    if assemble_process.returncode != 0:
        print(f"Error assembling {test_name}: {assemble_process.stderr.decode()}")
        clean_up()
        continue

    run_command = f"../out/ibssbi ./out"
    run_process = subprocess.run(run_command, shell=True, capture_output=True)

    output = run_process.stdout.decode().strip()
    expected = '\n'.join(map(str, expected_output))
    
    if run_process.stderr.decode().strip():
        print(f"Test {test_name} failed with an error.")
        print(run_process.stderr.decode().strip())
        clean_up()
        continue

    if output != expected:
        print(f"Test {test_name} failed: Expected [{', '.join(map(str, expected_output))}], but got [{', '.join(output.split('\n'))}]")
        output_str = "[" + ' '.join(map(str, expected_output)) + "]"
    else:
        print(f"Test {test_name} passed.")

    clean_up()

os.rmdir(temp_dir)
