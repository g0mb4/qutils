.PHONY:  clean common bspinfo light modelgen qbsp

all: bspinfo light modelgen qbsp

common:
	make -C common

bspinfo: common
	make -C bspinfo

light: common
	make -C light

modelgen: common
	make -C modelgen

qbsp: common
	make -C qbsp

clean:
	make clean -C common
	make clean -C bspinfo
	make clean -C light
	make clean -C modelgen
	make clean -C qbsp
