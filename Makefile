KERNEL := kernel.elf
ISO_IMAGE = disk.iso
FIRMWARE=legacy
CC := gcc
AS=nasm
LD=ld
OBJCOPY = objcopy
QEMU=qemu-system-x86_64
QEMUOPTIONS=-m 4096 -serial stdio -cdrom $(ISO_IMAGE) -no-reboot -no-shutdown
SRCDIR=src
BUILDDIR=bin
CFLAGS = -m64 -Wall -Wextra -O2 -pipe -g  -std=gnu99
ASFLAGS =-f elf64 -I$(SRCDIR)include/assembly
INTERNALLDFLAGS := \
	-m64 		\
	-nostdlib -shared -no-pie -fno-pic -z max-page-size=0x1000 \
	-T$(SRCDIR)/kernel/linker.ld    \
	

INTERNALCFLAGS  :=       \
	-I $(SRCDIR)/include \
	-m64				\
	-nostdlib -ffreestanding -mcmodel=kernel -fno-builtin -fno-stack-protector -fno-pic -mno-80387 -mno-mmx -mno-3dnow -mno-sse -mno-sse2 -mno-red-zone -mgeneral-regs-only

CFILES  = $(shell find $(SRCDIR) -name '*.c')
#OBJS   = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(COBJS))
OBJ = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(CFILES))
ASMFILES  = $(shell find $(SRCDIR) -name '*.s')
OBJ  += $(patsubst $(SRCDIR)/%.s,$(BUILDDIR)/%.s.o,$(ASMFILES))



.PHONY: all clean dirs

all: build

build:	dirs $(KERNEL)


$(KERNEL): $(OBJ) 
	$(CC) $(INTERNALLDFLAGS) $(OBJ) -o $@


$(BUILDDIR)/%.o: $(SRCDIR)/%.c 
	$(CC) $(CFLAGS) $(INTERNALCFLAGS) -c $< -o $@ 
$(BUILDDIR)/%.s.o: $(SRCDIR)/%.s
	@echo $<
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf $(BUILDDIR) $(ISO_IMAGE) $(KERNEL) iso_root
dirs:
	@mkdir -p $(BUILDDIR)
	@cd $(SRCDIR) \
	&& dirs=$$(find -type d) \
	&& cd ../$(BUILDDIR) \
	&& mkdir -p $$dirs


disk:  build
	rm -rf iso_root
	mkdir -p iso_root
	cp  $(KERNEL)  \
		config/limine.cfg limine/limine.sys limine/limine-cd.bin limine/limine-eltorito-efi.bin iso_root/
ifeq ($(FIRMWARE),uefi)
	mkdir iso_root/EFI; 
	mkdir iso_root/EFI/BOOT;
	cp limine/BOOTX64.EFI iso_root/EFI/BOOT; 
endif
	xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-eltorito-efi.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o $(ISO_IMAGE)


	@#dd if=/dev/zero of=test.img count=50 bs=512
	@#mformat -i test.img -f 2880 ::
	@#mcopy -i test.img $(KERNEL) :: # the kernel
	@#mcopy -i test.img config/limine.cfg limine/limine.sys limine/limine-cd.bin limine/limine-eltorito-efi.bin ::

	limine/limine-install $(ISO_IMAGE)
	rm -rf iso_root
run: disk
	$(QEMU) $(QEMUOPTIONS)
run-dbg: disk $(BUILDDIR)/kernel/kernel.dbg
	$(QEMU) $(QEMUOPTIONS) -s -S &
	@sleep 1
	@gdb -x ./qemu.dbg

$(BUILDDIR)/kernel/kernel.dbg: $(KERNEL)
	@$(OBJCOPY) --only-keep-debug $< $@