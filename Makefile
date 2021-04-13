SRCDIR   = ./src
BUILDDIR = ./build

# --------------------------------------------------------------------------- #

OBJS_ = $(shell find $(SRCDIR) -name '*.c')
OBJS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(OBJS_))

# --------------------------------------------------------------------------- #

.PHONY: build

# --------------------------------------------------------------------------- #

dirs:
	@mkdir -p $(BUILDDIR)
	@  cd $(SRCDIR) \
	&& dirs=$$(find -type d) \
	&& cd ../$(BUILDDIR) \
	&& mkdir -p $$dirs

build: dirs $(OBJS)
	@echo -e " [$(LD)]\tkernel)"
	@$(CC) -o $(BUILDDIR)/kernel $(OBJS)


# --------------------------------------------------------------------------- #

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@echo -e " [$(CC)]\t$(notdir $@)"
	@$(CC) -o $@ -c $<
