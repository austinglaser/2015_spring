.DELETE_ON_ERROR:

CC := gcc
CFLAGS := -Wextra -Wall -g -std=c99

ROOT_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
SRC_DIR := $(ROOT_DIR)/src
INC_DIR := $(ROOT_DIR)/inc
BUILD_DIR := $(ROOT_DIR)/build

all: hashtable_test

hashtable_test: $(BUILD_DIR)/unit_test.o $(BUILD_DIR)/hashtable_test.o $(BUILD_DIR)/hashtable.o | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -I$(INC_DIR) $^ -o $@

$(BUILD_DIR):
	mkdir -p $@

.PHONY: clean
clean:
	rm -rf hashtable_test $(BUILD_DIR)

.PHONY: test
test: hashtable_test
	valgrind -q --error-exitcode=128 ./$<

