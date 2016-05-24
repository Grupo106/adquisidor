PROGRAM = adquisidor
SRC_DIR = ./src
BUILD_DIR = ./build
BIN_DIR = ./bin

ECPG = ecpg
CC = gcc
COMPILE_FLAGS = -Wall -Wextra -g -O2
LINK_FLAGS = -lecpg

all: dirs $(BIN_DIR)/$(PROGRAM)

$(BIN_DIR)/$(PROGRAM): $(BUILD_DIR)/main.o $(BUILD_DIR)/db.o
	$(CC) $(LINK_FLAGS) -o $(BIN_DIR)/$(PROGRAM) $(BUILD_DIR)/db.o $(BUILD_DIR)/main.o

$(BUILD_DIR)/main.o: $(SRC_DIR)/db.h $(SRC_DIR)/main.c
	$(CC) $(COMPILE_FLAGS) -o $(BUILD_DIR)/main.o -c $(SRC_DIR)/main.c

$(BUILD_DIR)/db.o: $(SRC_DIR)/db.pgc $(SRC_DIR)/db.h $(SRC_DIR)/structures.h
	$(ECPG) -o $(SRC_DIR)/db.c $(SRC_DIR)/db.pgc
	$(CC) $(COMPILE_FLAGS) -o $(BUILD_DIR)/db.o -c $(SRC_DIR)/db.c

dirs:
	mkdir -p $(BUILD_DIR) $(BIN_DIR)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

#all: bin/adquisidor

#bin/adquisidor: src/adquisidor.c
#	gcc -o bin/adquisidor -lpcap src/adquisidor.c

#clean:
#	rm -rf bin/adquisidor
