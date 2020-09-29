#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'

BRED='\033[1;31m'
BGREEN='\033[1;32m'
BBLUE='\033[1;34m'

NC='\033[0m'

OUTFILE="toto"

# The goal here is to see if the compiler recognizes the syntax

clang -fsyntax-only -std=c99 interleaved_cilk.c -ftapir=cilk &> $OUTFILE
RC=$?
echo -n "Cilk"
if [ $RC != 0 ]; then
    echo -e "                               ${BRED}[FAILED]${NC}"
else
    echo -e "                               ${BGREEN}[PASSED]${NC}"
fi
clang++ -fsyntax-only forall2.cpp -ftapir=serial &> $OUTFILE
RC=$?
echo -n "Serial"
if [ $RC != 0 ]; then
    echo -e "                             ${BRED}[FAILED]${NC}"
else
    echo -e "                             ${BGREEN}[PASSED]${NC}"
fi
clang -fsyntax-only basic_openmp.c -ftapir=openmp &> $OUTFILE
RC=$?
echo -n "OpenMP"
if [ $RC != 0 ]; then
    echo -e "                             ${BRED}[FAILED]${NC}"
else
    echo -e "                             ${BGREEN}[PASSED]${NC}"
fi
