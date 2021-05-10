.PHONY: clean dirs

DIRS_ = $(OUTDIR) $(patsubst %,$(OUTDIR)/%,$(DIRS))

clean:
	@rm -rf $(OUTDIR)

dirs: $(DIRS_)

$(DIRS_):%:
	@mkdir -p $@

# ----------------------------------------------

$(OUTDIR)/%.o: $(SRCDIR)/%.c
	@echo -e " [$(CC)]\t$(notdir $@)"
	@$(CC) $(CFLAGS) -o $@ -c $<