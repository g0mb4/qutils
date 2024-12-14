.PHONY: clean common bspinfo light modelgen qbsp qcc qfiles qlumpy sprgen texmake vis

all: bspinfo light modelgen qbsp qcc qlumpy sprgen texmake vis

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

qfiles: common
	make -C $@

qlumpy: common
	make -C $@

sprgen: common
	make -C $@

texmake: common
	make -C $@

vis: common
	make -C $@

clean:
	make clean -C common
	make clean -C bspinfo
	make clean -C light
	make clean -C modelgen
	make clean -C qbsp
	make clean -C qcc
	make clean -C qfiles
	make clean -C qlumpy
	make clean -C sprgen
	make clean -C texmake
	make clean -C vis
