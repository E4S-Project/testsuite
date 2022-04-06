#!/bin/bash

#make

. ./setup.sh

#RED='\033[0;31m'
#GREEN='\033[0;32m'
#BLUE='\033[0;34m'

#BRED='\033[1;31m'
#BGREEN='\033[1;32m'
#BBLUE='\033[1;34m'

#NC='\033[0m'

OUTFILE="toto"

echo -e "${BBLUE}Simple transforms${NC}"
clang -O3 -mllvm -polly -mllvm -polly-process-unprofitable -Wall  -o permutation permutation.c -lm &> $OUTFILE
RET=$?
echo -n "Loop permutation  " 
if [ $RET == 0 ] ; then
    echo -e "                             ${BGREEN}[PASSED]${NC}"
else
    echo -e "                             ${BRED}[FAILED]${NC}"
fi

clang -O3 -mllvm -polly -mllvm -polly-process-unprofitable -Wall  -o reverse reverse.c -lm &> $OUTFILE
RET=$?
echo -n "Loop reverse  " 
if [ $RET == 0 ] ; then
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
else
    echo -e "                                 ${BRED}[FAILED]${NC}"
fi

clang -O3 -mllvm -polly -mllvm -polly-process-unprofitable -Wall  -o tile tile.c -lm &> $OUTFILE
RET=$?
echo -n "Loop tiling   " 
if [ $RET == 0 ] ; then
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
else
    echo -e "                                 ${BRED}[FAILED]${NC}"
fi

clang -O3 -mllvm -polly -mllvm -polly-process-unprofitable -Wall  -o unrolling unrolling.c -lm &> $OUTFILE
RET=$?
echo -n "Loop unrolling  " 
if [ $RET == 0 ] ; then
    echo -e "                               ${BGREEN}[PASSED]${NC}"
else
    echo -e "                               ${BRED}[FAILED]${NC}"
fi

echo -e "${BBLUE}Other kernels${NC}"

clang -O3  -Wall  -o matmul_unchanged matmul_unchanged.c -lm > $OUTFILE
clang -O3 -mllvm -polly -mllvm -polly-process-unprofitable -Wall  -o matmul matmul.c -lm &> $OUTFILE
RET=$?
echo -n "Matrix-matrix multiplication" 
if [ $RET == 0 ] ; then
    echo -e "                   ${BGREEN}[PASSED]${NC}"
else
    echo -e "                   ${BRED}[FAILED]${NC}"
fi

clang -O3 -mllvm -polly -mllvm -polly-process-unprofitable -Wall  -o crout crout.c -lm &> $OUTFILE
RET=$?
echo -n "Crout's factorization" 
if [ $RET == 0 ] ; then
    echo -e "                          ${BGREEN}[PASSED]${NC}"
else
    echo -e "                          ${BRED}[FAILED]${NC}"
fi


