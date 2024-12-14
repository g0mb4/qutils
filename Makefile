.PHONY: dir clean common bspinfo light modelgen qbsp qcc qfiles qlumpy \
        sprgen texmake vis

all: bspinfo light modelgen qbsp qcc qfiles qlumpy sprgen texmake vis

dir:
	@echo " MKDIR  bin"
	@mkdir -p bin

common:
	@make -C common

bspinfo: dir common
	@make -C $@
	@cp $@/$@ bin/$@

light: dir common
	@make -C $@
	@cp $@/$@ bin/$@

modelgen: dir common
	@make -C $@
	@cp $@/$@ bin/$@

qbsp: dir common
	@make -C $@
	@cp $@/$@ bin/$@

qcc: dir common
	@make -C $@
	@cp $@/$@ bin/$@

qfiles: dir common
	@make -C $@
	@cp $@/$@ bin/$@

qlumpy: dir common
	@make -C $@
	@cp $@/$@ bin/$@

sprgen: dir common
	@make -C $@
	@cp $@/$@ bin/$@

texmake: dir common
	@make -C $@
	@cp $@/$@ bin/$@

vis: dir common
	@make -C $@
	@cp $@/$@ bin/$@

clean:
	rm -rf bin/*
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
