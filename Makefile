.PHONY:  clean common bspinfo light

all: bspinfo light

common:
	make -C common

bspinfo: common
	make -C bspinfo

light: common
	make -C light

clean:
	make clean -C common
	make clean -C bspinfo
	make clean -C light
