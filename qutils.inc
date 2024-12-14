CC ?= gcc
CFLAGS ?= -std=c99 -I ../common

EXE ?=
SRC ?= $(wildcard *.c)
OBJS ?= $(SRC:.c=.o)
LIBS ?=

.PHONY: all clean

all: $(EXE)

%.o: %.c
	@echo "  CC    $@"
	@$(CC) $(CFLAGS) -c -o $@ $<

$(EXE): $(OBJS)
	@echo "  LD    $@"
	@$(CC) -o $@ $(OBJS) $(LIBS)

clean:
	rm -rf $(EXE)
	rm -rf *.o

