.PHONY:  clean common bspinfo light modelgen

all: bspinfo light modelgen

common:
	make -C common

bspinfo: common
	make -C bspinfo

light: common
	make -C light

modelgen: common
	make -C modelgen

clean:
	make clean -C common
	make clean -C bspinfo
	make clean -C light
	make clean -C modelgen
