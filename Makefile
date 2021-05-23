SRCDIR   = ./src
BUILDDIR = ./build

# --------------------------------------------------------------------------- #

CC = gcc
AS = nasm
LD = ld
OBJCOPY = objcopy
CFLAGS = -std=gnu99 -nostdlib -m64 -ffreestanding -fno-builtin -fno-stack-protector -fno-pic -fno-pie  -I./src/include -g -O2 -Wall -Wextra -mno-80387 -mno-mmx  -mno-3dnow -mno-sse -mno-sse2 -mno-red-zone
LFLAGS = -m64 -nostdlib -z max-page-size=0x1000 -fno-pic -fno-pie
ASFLAGS = -g -f elf64 -I./src/include/assembly
QEMU_OPTIONS = -soundhw pcspk -m 4096
IMG_DIR=img
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
	@rm -rf $(BUILDDIR) *.iso img/boot/*.img img/boot/kernel img/boot/grub/*.cfg $(IMG_DIR) tools/*.img tools/Genfs

qemu: $(BUILDDIR)/kernel/kernel
	@qemu-system-i386 $(QEMU_OPTIONS) -serial stdio -kernel $<

qemu-serial: $(BUILDDIR)/kernel/kernel
	@qemu-system-i386 $(QEMU_OPTIONS) -display none -serial stdio -kernel $<

dbg: $(BUILDDIR)/kernel/kernel $(BUILDDIR)/kernel/kernel.dbg
	@qemu-system-i386 $(QEMU_OPTIONS) -serial stdio -kernel $< -s -S &
	@sleep 1
	@gdb -x ./qemu.dbg
build-fsGen:
	@$(CC) -o ./tools/Genfs ./tools/fsGen.c
build-img-dir:
	-@mkdir $(IMG_DIR)
	-@mkdir $(IMG_DIR)/boot
	-@mkdir $(IMG_DIR)/boot/grub
build-image-ramdisk: build-img-dir build build-fsGen
	@cp $(BUILDDIR)/kernel/kernel $(IMG_DIR)/boot/kernel
	@cd ./tools && ./Genfs test.txt test.txt && pwd
	@cp ./tools/initrd.img $(IMG_DIR)/boot/initrd
	@cp grub/grub-ramdisk.cfg $(IMG_DIR)/boot/grub/grub.cfg
	@grub-mkrescue -o osakauss.iso $(IMG_DIR)

build-image: build-img-dir build
	@cp $(BUILDDIR)/kernel/kernel $(IMG_DIR)/boot/kernel
	cp -v $(BUILDDIR)/kernel/kernel bootloader/limine.cfg bootloader/limine/limine.sys \
      bootloader/limine/limine-cd.bin bootloader/limine/limine-eltorito-efi.bin img/
	xorriso -as mkisofs -b limine-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table \
        -eltorito-alt-boot -e limine-eltorito-efi.bin \
        -no-emul-boot img -o osakauss.iso


qemu-iso: build-image
	@qemu-system-x86_64 $(QEMU_OPTIONS) -serial stdio -cdrom osakauss.iso

qemu-iso-dbg: build-image
	@qemu-system-x86_64 $(QEMU_OPTIONS) -serial stdio -cdrom osakauss.iso -s -S &
	@sleep 1
	@gdb -x ./qemu.dbg

qemu-iso-ramdisk: build-image-ramdisk
	@qemu-system-x86_64 $(QEMU_OPTIONS) -serial stdio -cdrom osakauss.iso

qemu-iso-ramdisk-dbg: build-image-ramdisk $(BUILDDIR)/kernel/kernel.dbg
	@qemu-system-x86_64 $(QEMU_OPTIONS) -serial stdio -cdrom osakauss.iso -s -S &
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
	$(CC) $(LFLAGS) -T $(SRCDIR)/kernel/linker.ld -o $(BUILDDIR)/kernel/kernel.elf $(OBJS) $(BUILDDIR)/kernel/boot.S.o

# --------------------------------------------------------------------------- #

#$(BUILDDIR)/%.S.o: $(SRCDIR)/%.S
#	@echo -e " [$(AS)]\t$(notdir $@)"
#	@$(AS) -f elf32 $(ASFLAGS) -o $@ $<

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@echo -e " [$(CC)]\t$(notdir $@)"
	@$(CC) $(CFLAGS) -o $@ -c $<

# --------------------------------------------------------------------------- #