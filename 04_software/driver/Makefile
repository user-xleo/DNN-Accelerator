# Compiler and flags
CC = gcc
CFLAGS = -Wall -Werror -O2 -fPIC
CPPFLAGS = -I../hal/include -I./include

# Library name and version
LIB_NAME = libaccel_driver
LIB_VERSION = 1.0.0
SONAME = $(LIB_NAME).so.1

# Directory structure
SRC_DIR = src
OBJ_DIR = obj
TEST_DIR = test
LIB_DIR = lib

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_BINS = $(TEST_SRCS:$(TEST_DIR)/%.c=$(TEST_DIR)/%)

# Shared library files
LIB_SO = $(LIB_DIR)/$(LIB_NAME).so.$(LIB_VERSION)
LIB_SONAME = $(LIB_DIR)/$(LIB_NAME).so.1
LIB_LINK = $(LIB_DIR)/$(LIB_NAME).so

# Installation directories
PREFIX = /usr/local
LIBDIR = $(PREFIX)/lib
INCLUDEDIR = $(PREFIX)/include/accel

# HAL library path and file
HAL_LIB = ../hal/lib/libhal_accelerator.a

.PHONY: all clean install uninstall test

all: $(LIB_SO)

# Create directories
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(LIB_DIR):
	mkdir -p $(LIB_DIR)

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Create shared library
$(LIB_SO): $(OBJS) | $(LIB_DIR)
	$(CC) -shared -Wl,-soname,$(SONAME) -o $@ $(OBJS) $(HAL_LIB)
	cd $(LIB_DIR) && ln -sf $(LIB_NAME).so.$(LIB_VERSION) $(LIB_NAME).so.1
	cd $(LIB_DIR) && ln -sf $(LIB_NAME).so.1 $(LIB_NAME).so

# Build and run tests
test: $(TEST_BINS)
	for test in $(TEST_BINS); do ./$$test; done

$(TEST_DIR)/%: $(TEST_DIR)/%.c $(LIB_SO)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ -L$(LIB_DIR) -Wl,-rpath,$(shell pwd)/$(LIB_DIR) -laccel_driver ../hal/lib/libhal_accelerator.a

# Install library and headers
install: $(LIB_SO)
	install -d $(DESTDIR)$(LIBDIR)
	install -m 755 $(LIB_SO) $(DESTDIR)$(LIBDIR)
	ln -sf $(LIB_SO) $(DESTDIR)$(LIBDIR)/$(LIB_SONAME)
	ln -sf $(LIB_SONAME) $(DESTDIR)$(LIBDIR)/$(LIB_LINK)
	install -d $(DESTDIR)$(INCLUDEDIR)
	install -m 644 include/*.h $(DESTDIR)$(INCLUDEDIR)
	ldconfig

# Uninstall library and headers
uninstall:
	rm -f $(DESTDIR)$(LIBDIR)/$(LIB_SO)
	rm -f $(DESTDIR)$(LIBDIR)/$(LIB_SONAME)
	rm -f $(DESTDIR)$(LIBDIR)/$(LIB_LINK)
	rm -rf $(DESTDIR)$(INCLUDEDIR)

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(LIB_DIR)
	rm -f $(TEST_BINS)
