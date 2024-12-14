CC ?= gcc
CFLAGS ?= -std=c99 -I ../common

PROJECT ?=
OBJS ?=
LIBS ?=

%.o: %.c
	@echo "  CC    $@"
	@$(CC) $(CFLAGS) -c -o $@ $<

$(PROJECT): $(OBJS)
	@echo "  LD    $@"
	@$(CC) -o $@ $(OBJS) $(LIBS)

