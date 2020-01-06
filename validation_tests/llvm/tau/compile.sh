#!/bin/bash

export PATH=${LLVM_DIR}/bin:$PATH
export LD_LIBRARY_PATH=${LLVM_DIR}/lib:$LD_LIBRARY_PATH

export TAU=$HOME/x86/tau-2.28.2/x86_64/
export TAU_MAKEFILE=shared-clang-pdt

export TAU_OPTIONS='-optCompInst -optVerbose'

#which clang
clang++ -O3 -g -fplugin=${LLVM_DIR}/lib/TAU_Profiling_CXX.so -mllvm -tau-input-file=./functions_CXX_hh.txt -ldl -L${TAU}/lib/$TAU_MAKEFILE -lTAU -Wl,-rpath,${TAU}/lib/$TAU_MAKEFILE householder.cpp -o householder
clang++ -fjit -O3 -g -fplugin=${LLVM_DIR}/lib/TAU_Profiling_CXX.so -mllvm -tau-input-file=./functions_CXX_hh.txt -ldl -L${TAU}/lib/$TAU_MAKEFILE -lTAU -Wl,-rpath,${TAU}/lib/$TAU_MAKEFILE householder_jit.cpp -o householder_jit

clang -O3 -g -fplugin=${LLVM_DIR}/lib/TAU_Profiling.so -mllvm -tau-input-file=./functions_C_mm.txt -ldl -L${TAU}/lib/$TAU_MAKEFILE -lTAU -Wl,-rpath,${TAU}/lib/$TAU_MAKEFILE matmult.c matmult_initialize.c -o mm_c

clang -O3 -g -fplugin=${LLVM_DIR}/lib/TAU_Profiling_CXX.so -mllvm -tau-input-file=./functions_CXX_mm.txt -ldl -L${TAU}/lib/$TAU_MAKEFILE -lTAU -Wl,-rpath,${TAU}/lib/$TAU_MAKEFILE matmult.cpp matmult_initialize.cpp -o mm_cpp

