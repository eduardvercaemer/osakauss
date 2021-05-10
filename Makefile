include ./project.mk

OUTDIR := $(BUILDDIR)

### ----------------------------------------------------- ###

QEMU      := qemu-system-i386
QEMU_OPTIONS = -soundhw pcspk -m 4096

### ------------------- kernel -------------------------- ###

.PHONY: kernel kernel-dbg

# produce the stripped final kernel binary
kernel:
	@$(MAKE) --no-print-directory -C $(MAKEDIR)/kernel

# produce debugging symbols for the kernel
kernel-dbg:
	@$(MAKE) kernel-dbg --no-print-directory -C $(MAKEDIR)/kernel

.PHONY: dbg qemu serial

# start a simple debugging session in a tmux environment
dbg: kernel kernel-dbg
	@$(SCRIPTS)/dbg_session.sh

# start the kernel in qemu
qemu: kernel
	@$(QEMU) $(QEMU_OPTIONS) -kernel $(BUILDDIR)/kernel/kernel_stripped

# start the kernel in console with serial
serial: kernel
	@$(QEMU) $(QEMU_OPTIONS) -serial stdio -display none -kernel $(BUILDDIR)/kernel/kernel_stripped

include $(PROJECT)/rules.mk