EXE := modelgen
SRC := $(wildcard *.c)
OBJS := $(SRC:.c=.o)
LIBS := ../common/cmdlib.o \
	../common/mathlib.o \
	-lm \
	../common/trilib.o \
	../common/scriplib.o \
	../common/lbmlib.o

include ../qutils.inc
