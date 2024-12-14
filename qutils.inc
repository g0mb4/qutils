CC ?= gcc
CFLAGS ?= -std=c99 -I ../common

%.o: %.c
	@echo "  CC    $@"
	@$(CC) $(CFLAGS) -c -o $@ $<
