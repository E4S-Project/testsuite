#!/bin/bash

. ./setup.sh

#RED='\033[0;31m'
#GREEN='\033[0;32m'
#BLUE='\033[0;34m'

#BRED='\033[1;31m'
#BGREEN='\033[1;32m'
#BBLUE='\033[1;34m'

#NC='\033[0m'

OUTFILE=toto

echo -e "${BBLUE}Sanity checks (presence) ${NC}"
which clang &> $OUTFILE
RET=$?
echo -n "Clang present in the path  " 
if [ $RET == 0 ] ; then
    echo -e "                          ${BGREEN}[PASSED]${NC}"
else
    echo -e "                          ${BRED}[FAILED]${NC}"
    exit -1
fi

clang -v &> $OUTFILE
# grepping "clang" would be useless
echo -n "clang -v returns something expected  " 
RET=`grep "clang version" $OUTFILE | wc -l`
if [ $RET == 0 ] ; then
    echo -e "                ${BRED}[FAILED]${NC}"
    exit -1
else
    echo -e "                ${BGREEN}[PASSED]${NC}"
fi


echo -e "${BBLUE}Basic sanity checks (backend: tapir)${NC}"
clang -ftapir=openmp -fopenmp -o basic basic.c
RC=$?
echo -n "Basic OpenMP"
if [ $RC != 0 ]; then
    echo -e "                               ${BRED}[FAILED]${NC}"
else
    echo -e "                               ${BGREEN}[PASSED]${NC}"
fi

clang -ftapir=openmp -fopenmp -o basic2 basic2.c
RC=$?
echo -n "Basic OpenMP 2"
if [ $RC != 0 ]; then
    echo -e "                             ${BRED}[FAILED]${NC}"
else
    echo -e "                             ${BGREEN}[PASSED]${NC}"
fi

echo -e "${BBLUE}Backends${NC}"
cd compilation
./compile.sh
cd ..
