SRCDIR   = ./src
BUILDDIR = ./build

# --------------------------------------------------------------------------- #

CC = gcc
AS = nasm
LD = ld
CFLAGS =  -O2 -Wall -Wextra -nostdlib -m32 -ffreestanding -fno-pie -I./src/include  -O2 -Wall -Wextra
LFLAGS = -m elf_i386
ASFLAGS = -f elf32 -I./src/include/assembly

# --------------------------------------------------------------------------- #

COBJS  = $(shell find $(SRCDIR) -name '*.c')
OBJS   = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(COBJS))
SOBJS  = $(shell find $(SRCDIR) -name '*.S')
#OBJS  += $(patsubst $(SRCDIR)/%.S,$(BUILDDIR)/%.S.o,$(SOBJS))

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
	@$(AS) $(ASFLAGS) -o $(BUILDDIR)/kernel/boot.S.o $(SRCDIR)/kernel/boot.S
	@$(LD) $(LFLAGS) -T $(SRCDIR)/kernel/linker.ld -o $(BUILDDIR)/kernel/kernel $(OBJS) $(BUILDDIR)/kernel/boot.S.o 

clean:
	@rm -rf $(BUILDDIR)

qemu-serial: build
	qemu-system-i386 -display none -serial stdio -kernel $(BUILDDIR)/kernel/kernel

qemu: build
	qemu-system-i386 -serial stdio -kernel $(BUILDDIR)/kernel/kernel
# --------------------------------------------------------------------------- #


#$(BUILDDIR)/%.S.o: $(SRCDIR)/%.S
#	@echo -e " [$(AS)]\t$(notdir $@)"
#	@$(AS) -f elf32 $(ASFLAGS) -o $@ $<

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@echo -e " [$(CC)]\t$(notdir $@)"
	@$(CC) $(CFLAGS) -o $@ -c $<

# --------------------------------------------------------------------------- #
