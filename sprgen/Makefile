PROJECT := sprgen
SRC := $(wildcard *.c)
OBJS := $(SRC:.c=.o)
LIBS := ../common/cmdlib.o \
	../common/lbmlib.o \
	../common/scriplib.o \
	-lm

all: $(PROJECT)

clean:
	rm -rf *.o
	rm -rf $(PROJECT)

include ../qutils.inc
