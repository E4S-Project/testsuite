#!/bin/bash

module use $HOME/x86/modulefiles
module load llvm/pragmas/git python/3.6.8

export LLVM_DIR=~/x86/pragmas/llvm-project/install/
export TAU=$HOME/x86/tau2/x86_64
export TAU_MAKEFILE=shared-clang-ompt-v5-pdt-openmp
export TAU_FUNCTIONS=$PWD/functions_C.txt
