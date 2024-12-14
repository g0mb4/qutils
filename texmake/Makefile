PROJECT := texmake
SRC := $(wildcard *.c)
OBJS := $(SRC:.c=.o)
LIBS := ../common/cmdlib.o \
	../common/mathlib.o \
	-lm \
	../common/lbmlib.o \
	../common/trilib.o

all: $(PROJECT)

clean:
	rm -rf *.o
	rm -rf $(PROJECT)

include ../qutils.inc
