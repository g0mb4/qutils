.PHONY: common bspinfo clean

all: bspinfo

common:
	make -C common

bspinfo: common
	make -C bspinfo

clean:
	make clean -C common
	make clean -C bspinfo
