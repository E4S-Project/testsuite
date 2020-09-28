#!/bin/bash

. ./setup.sh

echo -e "${BLUE}[Basic sanity checks]${NC}"
clang -fopenmp -o basic basic.c
RC=$?
echo -n "Basic 1 "
if [ $? != 0 ]; then
    echo -e "                                 ${BRED}[FAILED]${NC}"
else
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
fi

clang -fopenmp -o basic2 basic2.c
RC=$?
echo -n "Basic 2 "
if [ $? != 0 ]; then
    echo -e "                                 ${BRED}[FAILED]${NC}"
else
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
fi

clang -std=c99 -O3 -march=native -fno-unroll-loops -ffast-math -mllvm -polly -o matmul matmul.c
RC=$?
echo -n "Matmul "
if [ $? != 0 ]; then
    echo -e "                                  ${BRED}[FAILED]${NC}"
else
    echo -e "                                  ${BGREEN}[PASSED]${NC}"
fi

cd pragmas
./compile.sh
cd ..
