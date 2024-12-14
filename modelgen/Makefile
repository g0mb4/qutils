PROJECT := modelgen
SRC := $(wildcard *.c)
OBJS := $(SRC:.c=.o)
LIBS := ../common/cmdlib.o \
	../common/mathlib.o \
	-lm \
	../common/trilib.o \
	../common/scriplib.o \
	../common/lbmlib.o

all: $(PROJECT)

clean:
	rm -rf *.o
	rm -rf $(PROJECT)

include ../qutils.inc
