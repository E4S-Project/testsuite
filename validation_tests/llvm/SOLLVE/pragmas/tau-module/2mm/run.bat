#!/bin/bash

export LLVM_DIR=~/x86/pragmas/llvm-project/install/
export TAU=$HOME/x86/tau2/x86_64
export TAU_MAKEFILE=shared-clang-ompt-v5-pdt-openmp
export TAU_FUNCTIONS=$PWD/functions_C.txt

python3 -m ytopt.search.ambs --evaluator ray --problem problem.Problem --max-evals=20 --learner RF 
python3 ../tools/findMin.py $PWD/results.csv
