PROJECT := bspinfo
SRC := $(wildcard *.c)
OBJS := $(SRC:.c=.o)

all: $(PROJECT)

$(PROJECT): $(OBJS)
	@echo "  LD    $@"
	@$(CC) -o $@ $(OBJS) ../common/cmdlib.o ../common/bspfile.o

clean:
	rm -rf *.o
	rm -rf $(PROJECT)

include ../qutils.inc
