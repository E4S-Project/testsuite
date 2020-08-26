cmake_minimum_required (VERSION 3.8)

project(prog C)

add_executable(prog prog.c)
add_library(add SHARED libadd.c)
target_link_libraries(prog LINK_PUBLIC add)
