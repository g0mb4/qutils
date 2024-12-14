# qutils

Quake Utilities from id.

## Why?

This verison allows qutils to be built on modern Linux systems.

Original source: https://github.com/id-Software/Quake-Tools/tree/master/qutils

## How?
```bash
make
```
everything will be in `./bin`.

## What?

`qbsp` / `light` / `vis`: these utilities are called directly from the map editor to process *.map* files into *.bsp* files.  They can be executed by hand if desired.

`bspinfo`: a command line utility that will dump the count and size statistics on a *.bsp* file.

`qlumpy`: the 2-D graphics grabber.  Grabs graphics off of .lbm pictures.  Used for grabbing the 2d graphics used by quake (status bar stuff, fonts, etc), and also used for grabbing the textures to be viewed in qe3 and extracted by qbsp.  Qlumpy script files have the default extension *".ls"* (LumpyScript).

`qcc`: the Quake-C compiler.  Reads *progs.src*, then compiles all of the files listed there.  Generates *progs.dat* for use by `quake` at runtime, *progdefs.h* for use at compile time, and *files.dat* to be used as input for `qfiles`.

`qfiles`: Builds pak files based on the contents of *files.dat* writen out by `qcc`.  It can also regenerate all of the *.bsp* models used in a project, which is required if any changes to the file format have been made.

`sprgen`: the sprite model grabber. Grabs 2d graphics and creates a *.spr* file.

`modelgen`: the 3-D model grabber.  Combines skin graphics with 3d frames to produce a *.mdl* file.  The commands are parsed out of *.qc* files that can also be read by `qcc`, so a single source can both generate and use the data.

`texmake`: creates 2d wireframe outlines of a 3d model that can be drawn on to give a texture to a model.  This is only done once per model, or when the base frame changes.
Example:
`cd ~/quake/id1/models/torch`
`texmake base`                  reads base.tri and creates the graphic base.lbm
`copy base.lbm skin.lbm`        never work on the base skin, it might get overwritten
`cd ~/quake/id1/progs`
`modelgen torch.qc`             creates torch.mdl out of files in ~/quake/id1/models/torch

## Example

```bash
cd ~/quake/id1/gfx
```

Regrab all 2d graphics:
```bash
for i in *.ls; do qlumpy $i; done
```
*gfx.ls*: graphics that are statically loaded: the small font, status bar stuff, etc
*cached.ls*: graphics that are dynamically cached: menus, console background, etc
the other *.ls* files are texture paletes for map editing

```bash
cd ~/quake/id1/progs
```

Regrab the sprites used in the 3d world (all three of them):
```bash
sprgen sprites.qc
```

Regrab all 3d models:
```bash
for i in *.qc; do modelgen $i; done
```
many of the *.qc* files do not actually specify a model, but running them through `modelgen` is harmless

Rebuild *progs.dat* and *files.dat*:
```bash
qcc
```

Read *files.dat* and runs `qbsp` and light on all external brush models (health boxes, ammo boxes, etc):
```bash
qfiles -bspmodels
```

Build *~/quake/id1/pak0.pak*:
```bash
qfiles -pak 0
```

Build *~/quake/id1/pak1.pak*:
```bash
qfiles -pak 1
```

note that you should not leave the pak files in your development directory, because you won't be able to override any of the contents. If you are doing your work in an add-on directory, it isn't a problem, and the pak files will load faster than the discrete files.

