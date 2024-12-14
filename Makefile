.PHONY:  clean common bspinfo light modelgen qbsp qcc

all: bspinfo light modelgen qbsp qcc

common:
	make -C common

bspinfo: common
	make -C $@

light: common
	make -C $@

modelgen: common
	make -C $@

qbsp: common
	make -C $@

qcc: common
	make -C $@

clean:
	make clean -C common
	make clean -C bspinfo
	make clean -C light
	make clean -C modelgen
	make clean -C qbsp
	make clean -C qcc
