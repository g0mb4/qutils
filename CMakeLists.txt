cmake_minimum_required(VERSION 3.1)
project(qutils C)

set(CMAKE_C_STANDARD 99)

#== id Software ==============================================================

# common libs
add_library(bspfile OBJECT common/bspfile.c)
add_library(cmdlib OBJECT common/cmdlib.c)
add_library(lbmlib OBJECT common/lbmlib.c)
add_library(mathlib OBJECT common/mathlib.c)
add_library(polylib OBJECT common/polylib.c)
add_library(scriplib OBJECT common/scriplib.c)
add_library(threads OBJECT common/threads.c)
add_library(trilib OBJECT common/trilib.c)
add_library(wadlib OBJECT common/wadlib.c)

# bspinfo
file(GLOB_RECURSE src bspinfo/*.c)
add_executable(bspinfo ${src})
target_include_directories(bspinfo PUBLIC ./common)
target_link_libraries(bspinfo cmdlib bspfile)

# light
file(GLOB_RECURSE src light/*.c)
add_executable(light ${src})
target_include_directories(light PUBLIC ./common ./light)
target_link_libraries(light cmdlib bspfile mathlib)
if(NOT WIN32)
  target_link_libraries(light m)
endif(NOT WIN32)

# modelgen
file(GLOB_RECURSE src modelgen/*.c)
add_executable(modelgen ${src})
target_include_directories(modelgen PUBLIC ./common ./modelgen)
target_link_libraries(modelgen cmdlib mathlib trilib scriplib lbmlib)
if(NOT WIN32)
  target_link_libraries(modelgen m)
endif(NOT WIN32)

# qbsp
file(GLOB_RECURSE src qbsp/*.c)
add_executable(qbsp ${src})
target_include_directories(qbsp PUBLIC ./common ./qbsp)
target_link_libraries(qbsp cmdlib bspfile mathlib)
if(NOT WIN32)
  target_link_libraries(qbsp m)
endif(NOT WIN32)

# qcc
file(GLOB_RECURSE src qcc/*.c)
add_executable(qcc ${src})
target_include_directories(qcc PUBLIC ./common ./qcc)
target_link_libraries(qcc cmdlib)

# qfiles
file(GLOB_RECURSE src qfiles/*.c)
add_executable(qfiles ${src})
target_include_directories(qfiles PUBLIC ./common)
target_link_libraries(qfiles cmdlib)

# qlumpy
file(GLOB_RECURSE src qlumpy/*.c)
add_executable(qlumpy ${src})
target_include_directories(qlumpy PUBLIC ./common ./qlumpy)
target_link_libraries(qlumpy cmdlib lbmlib wadlib scriplib)

# sprgen
file(GLOB_RECURSE src sprgen/*.c)
add_executable(sprgen ${src})
target_include_directories(sprgen PUBLIC ./common ./sprgen)
target_link_libraries(sprgen cmdlib lbmlib scriplib)
if(NOT WIN32)
  target_link_libraries(sprgen m)
endif(NOT WIN32)

# texmake
file(GLOB_RECURSE src texmake/*.c)
add_executable(texmake ${src})
target_include_directories(texmake PUBLIC ./common)
target_link_libraries(texmake cmdlib mathlib lbmlib trilib)
if(NOT WIN32)
  target_link_libraries(texmake m)
endif(NOT WIN32)

# vis
file(GLOB_RECURSE src vis/*.c)
add_executable(vis ${src})
target_include_directories(vis PUBLIC ./common)
target_link_libraries(vis cmdlib bspfile mathlib)
if(NOT WIN32)
  target_link_libraries(vis m)
endif(NOT WIN32)

#== gmb ======================================================================

# qpak
file(GLOB_RECURSE src qpak/*.c)
add_executable(qpak ${src})
target_include_directories(qpak PUBLIC ./common)
target_link_libraries(qpak cmdlib)

