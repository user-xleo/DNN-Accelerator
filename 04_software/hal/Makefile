# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror -O2
INCLUDE = -Iinclude -Itest

# Directories
SRC_DIR = src
INC_DIR = include
TEST_DIR = test
OBJ_DIR = obj
LIB_DIR = lib

# Library name and path
LIB_NAME = hal_accelerator
LIB = $(LIB_DIR)/lib$(LIB_NAME).a

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Test files
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_BINS = $(TEST_SRCS:$(TEST_DIR)/%.c=$(TEST_DIR)/bin/%)

# Dependencies
HAL_DEPS = hal_base.o hal_config.o hal_io.o hal_mem.o

.PHONY: all clean test dirs

all: dirs $(LIB)

# Create necessary directories
dirs:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(LIB_DIR)
	@mkdir -p $(TEST_DIR)/bin

# Build object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $<

# Build static library
$(LIB): $(addprefix $(OBJ_DIR)/,$(HAL_DEPS))
	ar rcs $@ $^

# Individual test targets
$(TEST_DIR)/bin/%: $(TEST_DIR)/%.c $(LIB)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ $< $(LIB)

# Build and run tests
test: dirs $(TEST_DIR)/bin/test_hal_mem $(TEST_DIR)/bin/test_hal_io $(TEST_DIR)/bin/test_hal_init
	@echo "Running tests..."
	@for test in $(TEST_DIR)/bin/*; do \
		if [ -x $$test ]; then \
			./$$test; \
		fi \
	done

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(LIB_DIR)
	rm -rf $(TEST_DIR)/bin

# Show help
help:
	@echo "Available targets:"
	@echo "  all        - Build the HAL library (default)"
	@echo "  test       - Build and run tests"
	@echo "  clean      - Remove build artifacts"
	@echo "  help       - Show this help message"
