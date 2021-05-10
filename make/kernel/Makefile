include ../../project.mk

SRCDIR := $(PROJECT)/src
OUTDIR := $(BUILDDIR)/kernel
DIRS   = $(shell cd $(SRCDIR) && find -type d)

# --------------------------------------------------------------------------- #

CC = gcc
AS = nasm
LD = ld
OBJCOPY = objcopy
CFLAGS = -std=gnu99 -nostdlib -m32 -ffreestanding -fno-pie -fno-builtin -fno-stack-protector -I$(SRCDIR)/include -g -O2 -Wall -Wextra
LFLAGS = -m elf_i386
ASFLAGS = -g -f elf32 -I$(SRCDIR)/include/assembly
QEMU_OPTIONS = -soundhw pcspk -m 4096

# --------------------------------------------------------------------------- #

OBJS_  = $(shell find $(SRCDIR) -name '*.c')
OBJS   = $(patsubst $(SRCDIR)/%.c,$(OUTDIR)/%.o,$(OBJS_))

# --------------------------------------------------------------------------- #

.PHONY: kernel-stripped kernel-elf kernel-dbg dbg

kernel-stripped: dirs $(OUTDIR)/kernel_stripped

kernel-elf: dirs $(OUTDIR)/kernel_elf

kernel-dbg: dirs $(OUTDIR)/kernel_dbg

# --------------------------------------------------------------------------- #

$(OUTDIR)/kernel_stripped: $(OUTDIR)/kernel_elf
	@echo -e " [$(OBJCOPY)]\tkernel_stripped"
	@$(OBJCOPY) --strip-unneeded $< $@

$(OUTDIR)/kernel_elf: $(OBJS)
	@echo -e " [$(AS)]\tboot.S.o"
	@$(AS) $(ASFLAGS) -o $(OUTDIR)/kernel/boot.S.o $(SRCDIR)/kernel/boot.S
	@echo -e " [$(LD)]\tkernel_elf"
	@$(LD) $(LFLAGS) -T $(SRCDIR)/kernel/linker.ld -o $@ $(OBJS) $(OUTDIR)/kernel/boot.S.o

$(OUTDIR)/kernel_dbg: $(OUTDIR)/kernel_elf
	@echo -e " [$(OBJCOPY)]\tkernel.dbg"
	@$(OBJCOPY) --only-keep-debug $< $@

# --------------------------------------------------------------------------- #

include ../../rules.mk