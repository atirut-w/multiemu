# Directories
SRCDIR := src
BUILDDIR := build
OBJDIR := $(BUILDDIR)/obj
PREFIX := /usr/z80-elf

# Compiler and tools
CC := ez80-clang
AS := z80-elf-as
LD := z80-elf-ld
OBJCOPY := z80-elf-objcopy

# Compiler flags
INCLUDE := -I$(PREFIX)/include -I$(SRCDIR)/include -I$(SRCDIR)
CFLAGS := -target z80 -S -nostdinc --std=c90 -pedantic-errors -Os -mllvm -z80-gas-style $(INCLUDE)
ASFLAGS := $(INCLUDE)
LDFLAGS := -T linker.ld
LDLIBS := -L$(PREFIX)/lib -lc

# Source files
CSRC := $(shell find $(SRCDIR) -name '*.c')
ASRC := $(shell find $(SRCDIR) -name '*.s')

# Object files
COBJ := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(CSRC))
AOBJ := $(patsubst $(SRCDIR)/%.s,$(OBJDIR)/%.o,$(ASRC))
OBJS := $(COBJ) $(AOBJ)

# Main target
TARGET := $(BUILDDIR)/bootrom.bin

.PHONY: all clean

all: $(BUILDDIR)/bootrom.bin

clean:
	rm -rf $(BUILDDIR)/*

install:
	cp $(BUILDDIR)/libc.a $(PREFIX)/lib
	cp $(INCLUDE)/* $(PREFIX)/include

$(BUILDDIR) $(OBJDIR):
	mkdir -p $@

$(BUILDDIR)/bootrom.bin: $(BUILDDIR)/bootrom.elf
	$(OBJCOPY) -O binary $< $@

$(BUILDDIR)/bootrom.elf: $(OBJS) | $(BUILDDIR)
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.s | $(OBJDIR)
	$(AS) $(ASFLAGS) -o $@ $<

# The (e)Z80 compiler doesn't assemble for us, so we need to do it ourselves
$(OBJDIR)/%.o: $(OBJDIR)/%.s
	$(AS) $(ASFLAGS) -march=ez80 -o $@ $<

$(OBJDIR)/%.s: $(SRCDIR)/%.c | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $<
