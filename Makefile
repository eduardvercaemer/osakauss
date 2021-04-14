SRCDIR   = ./src
BUILDDIR = ./build

# --------------------------------------------------------------------------- #

CC = gcc
AS = nasm
LD = ld
CFLAGS = -nostdlib -m32 -ffreestanding -fno-pie
LFLAGS = -m elf_i386

# --------------------------------------------------------------------------- #

COBJS  = $(shell find $(SRCDIR) -name '*.c')
OBJS   = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(COBJS))
SOBJS  = $(shell find $(SRCDIR) -name '*.S')
OBJS  += $(patsubst $(SRCDIR)/%.S,$(BUILDDIR)/%.S.o,$(SOBJS))

# --------------------------------------------------------------------------- #

.PHONY: dirs build clean qemu

dirs:
	@mkdir -p $(BUILDDIR)
	@  cd $(SRCDIR) \
	&& dirs=$$(find -type d) \
	&& cd ../$(BUILDDIR) \
	&& mkdir -p $$dirs

build: dirs $(OBJS)
	@echo -e " [$(LD)]\tkernel"
	@$(LD) $(LFLAGS) -T $(SRCDIR)/linker.ld -o $(BUILDDIR)/kernel $(OBJS)

clean:
	@rm -rf $(BUILDDIR)

qemu: build
	qemu-system-i386 -kernel $(BUILDDIR)/kernel

# --------------------------------------------------------------------------- #

$(BUILDDIR)/%.S.o: $(SRCDIR)/%.S
	@echo -e " [$(AS)]\t$(notdir $@)"
	@$(AS) -f elf32 -o $@ $<

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@echo -e " [$(CC)]\t$(notdir $@)"
	@$(CC) $(CFLAGS) -o $@ -c $<

# --------------------------------------------------------------------------- #
