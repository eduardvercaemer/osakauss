SRCDIR   = ./src
BUILDDIR = ./build

# --------------------------------------------------------------------------- #

CC = gcc
AS = nasm
LD = ld
OBJCOPY = objcopy
CFLAGS = -std=gnu99 -nostdlib -m32 -ffreestanding -fno-pie -fno-builtin -fno-stack-protector -I./src/include -g -O2 -Wall -Wextra
LFLAGS = -m elf_i386
ASFLAGS = -g -f elf32 -I./src/include/assembly
QEMU_OPTIONS = -soundhw pcspk -m 4096

# --------------------------------------------------------------------------- #

COBJS  = $(shell find $(SRCDIR) -name '*.c')
OBJS   = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(COBJS))
#SOBJS  = $(shell find $(SRCDIR) -name '*.S')
#OBJS  += $(patsubst $(SRCDIR)/%.S,$(BUILDDIR)/%.S.o,$(SOBJS))

# --------------------------------------------------------------------------- #

.PHONY: all dirs build clean qemu qemu-serial dbg

all: build

dirs:
	@mkdir -p $(BUILDDIR)
	@  cd $(SRCDIR) \
	&& dirs=$$(find -type d) \
	&& cd ../$(BUILDDIR) \
	&& mkdir -p $$dirs

build: $(BUILDDIR)/kernel/kernel

clean:
	@rm -rf $(BUILDDIR)

qemu: $(BUILDDIR)/kernel/kernel
	@qemu-system-i386 $(QEMU_OPTIONS) -serial stdio -kernel $<

qemu-serial: $(BUILDDIR)/kernel/kernel
	@qemu-system-i386 $(QEMU_OPTIONS) -display none -serial stdio -kernel $<

dbg: $(BUILDDIR)/kernel/kernel $(BUILDDIR)/kernel/kernel.dbg
	@qemu-system-i386 $(QEMU_OPTIONS) -serial stdio -kernel $< -s -S &
	@sleep 1
	@gdb -x ./qemu.dbg

# --------------------------------------------------------------------------- #

$(BUILDDIR)/kernel/kernel: dirs $(BUILDDIR)/kernel/kernel.elf
	@echo -e " [$(OBJCOPY)]\tkernel"
	@$(OBJCOPY) --strip-unneeded $(BUILDDIR)/kernel/kernel.elf $@

$(BUILDDIR)/kernel/kernel.dbg: $(BUILDDIR)/kernel/kernel.elf
	@echo -e " [$(OBJCOPY)]\tkernel.dbg"
	@$(OBJCOPY) --only-keep-debug $< $@

$(BUILDDIR)/kernel/kernel.elf: $(OBJS)
	@echo -e " [$(AS)]\tboot.S.o"
	@$(AS) $(ASFLAGS) -o $(BUILDDIR)/kernel/boot.S.o $(SRCDIR)/kernel/boot.S
	@echo -e " [$(LD)]\tkernel.elf"
	@$(LD) $(LFLAGS) -T $(SRCDIR)/kernel/linker.ld -o $(BUILDDIR)/kernel/kernel.elf $(OBJS) $(BUILDDIR)/kernel/boot.S.o

# --------------------------------------------------------------------------- #

#$(BUILDDIR)/%.S.o: $(SRCDIR)/%.S
#	@echo -e " [$(AS)]\t$(notdir $@)"
#	@$(AS) -f elf32 $(ASFLAGS) -o $@ $<

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@echo -e " [$(CC)]\t$(notdir $@)"
	@$(CC) $(CFLAGS) -o $@ -c $<

# --------------------------------------------------------------------------- #