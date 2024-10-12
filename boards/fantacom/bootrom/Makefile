# === Configuration ===

# Directories
SRC_DIR := src
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj

# Source files
C_SOURCES := $(wildcard $(SRC_DIR)/*.c)
ASM_SOURCES := $(wildcard $(SRC_DIR)/*.s)
SOURCES := $(C_SOURCES) $(ASM_SOURCES)

# Object files
C_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_SOURCES))
ASM_OBJECTS := $(patsubst $(SRC_DIR)/%.s,$(OBJ_DIR)/%.o,$(ASM_SOURCES))
OBJECTS := $(C_OBJECTS) $(ASM_OBJECTS)

# Compiler and tools
CC := ez80-clang
LD := z80-elf-ld
OBJCOPY := z80-elf-objcopy
AS := z80-elf-as

# Flags
CFLAGS := -target z80-unknown-elf -S
ASFLAGS := -I $(SRC_DIR)
LDFLAGS := -T linker.ld

# Target binary
TARGET := $(BUILD_DIR)/bootrom.bin

# === Targets ===

.PHONY: all clean

# Default target
all: $(TARGET)

# Convert ELF to binary
$(TARGET): $(BUILD_DIR)/bootrom.elf | $(BUILD_DIR)
	$(OBJCOPY) -O binary $< $@
	@echo "Generated $(TARGET)"

# Link ELF binary
$(BUILD_DIR)/bootrom.elf: $(OBJECTS) linker.ld | $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)
	@echo "Linked $@"

# Compile C source files to assembly using SDCC
$(OBJ_DIR)/%.s: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -o $@ $<
# Fix up directives
	sed -i 's/public/\.global/g' $@
	@echo "Compiled $< to $@"

# Assemble assembly files generated from C sources
$(OBJ_DIR)/%.o: $(OBJ_DIR)/%.s
	$(AS) $(ASFLAGS) -sdcc -o $@ $<
	@echo "Assembled $@ from $<"

# Assemble existing assembly source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s | $(OBJ_DIR)
	$(AS) $(ASFLAGS) -o $@ $<
	@echo "Assembled $@ from $<"

# Create necessary directories
$(OBJ_DIR):
	mkdir -p $@

$(BUILD_DIR):
	mkdir -p $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET)
	@echo "Cleaned build artifacts"
