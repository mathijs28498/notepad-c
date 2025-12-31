CC = gcc
CFLAGS = -Wall -Wextra -g -std=c17 -DINITGUID
LDFLAGS = -lgdi32 -luser32 -ld2d1 -ldwrite -lole32 -luuid -municode

VULKAN_SDK = $(shell echo $$VULKAN_SDK)

SRC_DIR = src
LIB_DIR = lib
BUILD_DIR = build
BIN_DIR = bin
TARGET = $(BIN_DIR)/program

rwildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*/),$(call rwildcard,$d,$2))

# Source files
SOURCES = $(call rwildcard,$(SRC_DIR)/,*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Library files
LIB_SOURCES = $(call rwildcard,$(LIB_DIR)/,*.c)
LIB_OBJECTS = $(LIB_SOURCES:$(LIB_DIR)/%.c=$(BUILD_DIR)/lib/%.o)

# Include directories (finds all directories containing .h files)
INC_DIRS := $(SRC_DIR) $(shell find $(LIB_DIR) -type d 2>/dev/null)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# Add include flags to CFLAGS
CFLAGS += $(INC_FLAGS)

all: $(TARGET)

$(TARGET): $(OBJECTS) $(LIB_OBJECTS)
	mkdir -p $(BIN_DIR)
	$(CC) $(OBJECTS) $(LIB_OBJECTS) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/lib/%.o: $(LIB_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean
