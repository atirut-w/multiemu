# === Configuration ===

# Directories
SRC_DIR := src
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj

# Source files
C_SOURCES := $(shell find $(SRC_DIR) -name '*.c')
ASM_SOURCES := $(shell find $(SRC_DIR) -name '*.s')
SOURCES := $(C_SOURCES) $(ASM_SOURCES)

# Object files
C_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_SOURCES))
ASM_OBJECTS := $(patsubst $(SRC_DIR)/%.s,$(OBJ_DIR)/%.o,$(ASM_SOURCES))
OBJECTS := $(C_OBJECTS) $(ASM_OBJECTS)

# Compiler and tools
CC := sdcc-sdcc
AS := z80-elf-as
LD := z80-elf-ld
OBJCOPY := z80-elf-objcopy

# Flags
CFLAGS := -mz80 -S -I$(SRC_DIR)/include --codeseg .text --constseg .rodata --dataseg .data
ASFLAGS := -I$(SRC_DIR)
LDFLAGS := -T linker.ld

# Main target
TARGET := $(BUILD_DIR)/bootrom.bin

# === Rules ===

.PHONY: all clean

all: $(BUILD_DIR)/bootrom.bin

clean:
	rm -rf $(BUILD_DIR)

$(TARGET): $(BUILD_DIR)/bootrom.elf
	$(OBJCOPY) -O binary $^ $@

$(BUILD_DIR)/bootrom.elf: $(OBJECTS) | $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $@ $^

$(BUILD_DIR) $(OBJ_DIR):
	mkdir -p $@

$(OBJ_DIR)/%.s: $(SRC_DIR)/%.c | $(OBJ_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $<
	sed -i 's/_\([a-zA-Z0-9_]*\)/\1/g' $@

$(OBJ_DIR)/%.o: $(OBJ_DIR)/%.s | $(OBJ_DIR)
	$(AS) -sdcc -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s | $(OBJ_DIR)
	mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -o $@ $<
