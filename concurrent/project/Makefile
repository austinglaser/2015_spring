.DELETE_ON_ERROR:

CC := gcc
CFLAGS := -Wextra -Wall -g -std=c11 -pthread

ROOT_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
SRC_DIR := $(ROOT_DIR)/src
INC_DIR := $(ROOT_DIR)/inc
BUILD_DIR := $(ROOT_DIR)/build

OBJS := $(BUILD_DIR)/unit_test.o $(BUILD_DIR)/hashtable.o $(BUILD_DIR)/hashtable_node.o

all: hashtable_test hashtable_node_test

hashtable_test: $(BUILD_DIR)/unit_test.o $(BUILD_DIR)/hashtable.o $(BUILD_DIR)/hashtable_node.o $(BUILD_DIR)/hashtable_test.o | $(BUILD_DIR)
	@echo "Linking $@"
	@$(CC) $(CFLAGS) $^ -o $@ -lpthread
	@echo ""
	@echo "Done Compiling"

hashtable_node_test: $(BUILD_DIR)/unit_test.o $(BUILD_DIR)/hashtable_node.o $(BUILD_DIR)/hashtable_node_test.o | $(BUILD_DIR)
	@echo "Linking $@"
	@$(CC) $(CFLAGS) $^ -o $@
	@echo ""
	@echo "Done Compiling"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CFLAGS) -c -I$(INC_DIR) $^ -o $@

$(BUILD_DIR):
	@echo "Compiler options:"
	@@echo "$(CC) $(CFLAGS) -c -I$(INC_DIR) main.c -o main.o"
	@echo ""
	mkdir -p $@
	@echo ""

.PHONY: clean
clean:
	@echo "Cleaning"
	@echo ""
	rm -rf hashtable_test $(BUILD_DIR)
	@echo ""
	@echo "Done Cleaning"

.PHONY: test
test: hashtable_test hashtable_node_test
	@echo "Testing"
	@valgrind -q --leak-check=full --error-exitcode=128 ./hashtable_node_test
	@valgrind -q --leak-check=full --error-exitcode=128 ./hashtable_test
	@echo "Done Testing"

