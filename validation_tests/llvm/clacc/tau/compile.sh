#!/bin/bash

export TAU=$HOME/x86/tau-2.28.2/x86_64/
export TAU_MAKEFILE=shared-clang-pdt

export TAU_OPTIONS='-optCompInst -optVerbose'

which clang
CLANGPATH=`which clang` 
NAME=${CLANGPATH%/*}       # remove 'clang'
LLVM_DIR=${NAME%/*}   # remove 'bin'
echo $LLVM_DIR

clang -O3 -g -fplugin=${LLVM_DIR}/lib/TAU_Profiling.so -mllvm -tau-input-file=./functions_hh.txt -ldl -L${TAU}/lib/$TAU_MAKEFILE -lTAU -Wl,-rpath,${TAU}/lib/$TAU_MAKEFILE householder.c -o householder -lm
