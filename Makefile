PROGRAM = main
SRC_DIR = ./src
BUILD_DIR = ./build
BIN_DIR = ./bin

ECPG = ecpg
CC = gcc
COMPILE_FLAGS = -Wall -Wextra -g -O2
LINK_FLAGS = -lecpg -lpcap

all: dirs $(BIN_DIR)/$(PROGRAM) $(BIN_DIR)/adquisidor

$(BIN_DIR)/$(PROGRAM): $(BUILD_DIR)/main.o $(BUILD_DIR)/db.o
	$(CC) $(LINK_FLAGS) -o $(BIN_DIR)/$(PROGRAM) $(BUILD_DIR)/db.o $(BUILD_DIR)/main.o

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c
	$(CC) $(COMPILE_FLAGS) $(INCLUDES_FLAGS) -o $(BUILD_DIR)/main.o -c $(SRC_DIR)/main.c

$(BUILD_DIR)/db.o: $(SRC_DIR)/db.pgc
	$(ECPG) -o $(BUILD_DIR)/db.c $(SRC_DIR)/db.pgc
	$(CC) $(COMPILE_FLAGS) -I$(SRC_DIR) -o $(BUILD_DIR)/db.o -c $(BUILD_DIR)/db.c

dirs:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

#all: bin/adquisidor

$(BIN_DIR)/adquisidor: $(SRC_DIR)/adquisidor.c
	gcc $(COMPILE_FLAGS) $(LINK_FLAGS) -o $(BIN_DIR)/adquisidor $(SRC_DIR)/adquisidor.c

#clean:
#	rm -rf bin/adquisidor
