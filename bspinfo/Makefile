PROJECT := bspinfo
SRC := $(wildcard *.c)
OBJS := $(SRC:.c=.o)
LIBS := ../common/cmdlib.o \
	../common/bspfile.o

all: $(PROJECT)

clean:
	rm -rf *.o
	rm -rf $(PROJECT)

include ../qutils.inc
