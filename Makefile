CC = gcc
CFLAGS = -Wall -Wextra -O2

SRC_DIR = ./src
OUT_DIR = ./out

TARGET = $(OUT_DIR)/ibssbi
SRC = $(SRC_DIR)/main.c $(SRC_DIR)/vm.c

$(TARGET): $(SRC)
	mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -rf $(OUT_DIR)

test_script:
	mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) ./src/create_test_program.c -o ./out/create_test_program