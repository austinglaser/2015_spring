.DELETE_ON_ERROR:

CC := gcc
CFLAGS := -Wextra -Wall -g -std=c11

ROOT_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
SRC_DIR := $(ROOT_DIR)/src
INC_DIR := $(ROOT_DIR)/inc
BUILD_DIR := $(ROOT_DIR)/build

all:		$(BUILD_DIR)/hashtable_test \
		$(BUILD_DIR)/hashtable_node_test \
		$(BUILD_DIR)/reference_list_test \
		$(BUILD_DIR)/reference_list_node_test \
		$(BUILD_DIR)/hashtable_benchmark

$(BUILD_DIR)/hashtable_test:		$(BUILD_DIR)/unit_test.o \
					$(BUILD_DIR)/hashtable.o \
					$(BUILD_DIR)/hashtable_node.o \
					$(BUILD_DIR)/hashtable_test.o \
					$(BUILD_DIR)/reference_list.o \
					$(BUILD_DIR)/reference_list_node.o \
					| $(BUILD_DIR)
	@echo "Linking $(notdir $@)"
	@$(CC) $(CFLAGS) $^ -o $@ -lpthread

$(BUILD_DIR)/hashtable_node_test:	$(BUILD_DIR)/unit_test.o \
					$(BUILD_DIR)/hashtable_node.o \
					$(BUILD_DIR)/hashtable_node_test.o \
					| $(BUILD_DIR)
	@echo "Linking $(notdir $@)"
	@$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/reference_list_test:	$(BUILD_DIR)/unit_test.o \
					$(BUILD_DIR)/reference_list.o \
					$(BUILD_DIR)/reference_list_node.o \
					$(BUILD_DIR)/reference_list_test.o \
					| $(BUILD_DIR)
	@echo "Linking $(notdir $@)"
	@$(CC) $(CFLAGS) $^ -o $@ -lpthread

$(BUILD_DIR)/reference_list_node_test:	$(BUILD_DIR)/unit_test.o \
					$(BUILD_DIR)/reference_list_node.o \
					$(BUILD_DIR)/reference_list_node_test.o \
					| $(BUILD_DIR)
	@echo "Linking $(notdir $@)"
	@$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/hashtable_benchmark:	$(BUILD_DIR)/hashtable_benchmark.o \
					$(BUILD_DIR)/hashtable.o \
					$(BUILD_DIR)/hashtable_node.o \
					$(BUILD_DIR)/reference_list.o \
					$(BUILD_DIR)/reference_list_node.o \
					| $(BUILD_DIR)
	@echo "Linking $(notdir $@)"
	@$(CC) $(CFLAGS) $^ -o $@ -lpthread

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
	rm -rf $(BUILD_DIR)
	@echo ""
	@echo "Done Cleaning"

.PHONY: test
test: $(BUILD_DIR)/hashtable_test $(BUILD_DIR)/hashtable_node_test $(BUILD_DIR)/reference_list_test $(BUILD_DIR)/reference_list_node_test
	@echo "Testing"
	@valgrind -q --leak-check=full --error-exitcode=128 $(BUILD_DIR)/reference_list_node_test
	@valgrind -q --leak-check=full --error-exitcode=128 $(BUILD_DIR)/reference_list_test
	@valgrind -q --leak-check=full --error-exitcode=128 $(BUILD_DIR)/hashtable_node_test
	@valgrind -q --leak-check=full --error-exitcode=128 $(BUILD_DIR)/hashtable_test
	@echo "Done Testing"

.PHONY: benchmark
benchmark: $(BUILD_DIR)/hashtable_benchmark
	@echo "Benchmarking"
	@echo ""
	@valgrind -q --leak-check=full --error-exitcode=128 $<
	@echo ""
	@echo "Done Benchmarking"

