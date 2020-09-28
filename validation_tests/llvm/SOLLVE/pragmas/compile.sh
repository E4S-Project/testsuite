#!/bin/bash

. ../setup.sh

# test on just one benchmark

cd crout
clang -DMEDIUM_DATASET -std=c99 -fno-unroll-loops -O3 -mllvm -polly -mllvm -polly-process-unprofitable -mllvm -polly-use-llvm-names -ffast-math -march=native -o crout crout.c polybench.c
RC=$?
echo -n "Crout   "
if [ $? != 0 ]; then
    echo -e "                                 ${BRED}[FAILED]${NC}"
else
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
fi

cd ..
