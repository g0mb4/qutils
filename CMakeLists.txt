cmake_minimum_required(VERSION 3.1)
project(qutils C)

set(CMAKE_C_STANDARD 99)

# common
file(GLOB_RECURSE src common/*.c)
add_library(common ${src})

# bspinfo
file(GLOB_RECURSE src bspinfo/*.c)
add_executable(bspinfo ${src})
target_include_directories(bspinfo PUBLIC ./common)
target_link_libraries(bspinfo common)

# light
file(GLOB_RECURSE src light/*.c)
add_executable(light ${src})
target_include_directories(light PUBLIC ./common ./light)
target_link_libraries(light common)

# modelgen
file(GLOB_RECURSE src modelgen/*.c)
add_executable(modelgen ${src})
target_include_directories(modelgen PUBLIC ./common ./modelgen)
target_link_libraries(modelgen common)

# qbsp
file(GLOB_RECURSE src qbsp/*.c)
add_executable(qbsp ${src})
target_include_directories(qbsp PUBLIC ./common ./qbsp)
target_link_libraries(qbsp common)

# qbsp
file(GLOB_RECURSE src qcc/*.c)
add_executable(qcc ${src})
target_include_directories(qcc PUBLIC ./common ./qcc)
target_link_libraries(qcc common)

# qfiles
file(GLOB_RECURSE src qfiles/*.c)
add_executable(qfiles ${src})
target_include_directories(qfiles PUBLIC ./common)
target_link_libraries(qfiles common)

# qlumpy
file(GLOB_RECURSE src qlumpy/*.c)
add_executable(qlumpy ${src})
target_include_directories(qlumpy PUBLIC ./common ./qlumpy)
target_link_libraries(qlumpy common)

# sprgen
file(GLOB_RECURSE src sprgen/*.c)
add_executable(sprgen ${src})
target_include_directories(sprgen PUBLIC ./common ./sprgen)
target_link_libraries(sprgen common)

# texmake
file(GLOB_RECURSE src texmake/*.c)
add_executable(texmake ${src})
target_include_directories(texmake PUBLIC ./common)
target_link_libraries(texmake common)

# vis
file(GLOB_RECURSE src vis/*.c)
add_executable(vis ${src})
target_include_directories(vis PUBLIC ./common)
target_link_libraries(vis common)
