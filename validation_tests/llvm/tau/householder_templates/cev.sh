#!/bin/bash

clang++  -fplugin=${LLVM_DIR}/lib/TAU_Profiling_CXX.so -mllvm -tau-input-file=./functions_CXX_hh.txt -ldl -L${TAU}/lib/$TAU_MAKEFILE -lTAU -Wl,-rpath,${TAU}/lib/$TAU_MAKEFILE  -O3 -o householder householder.cpp 
tau_exec -T serial,clang ./householder 256 256
pprof 

