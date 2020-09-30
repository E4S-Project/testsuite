#!/bin/bash

. ./setup.sh

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'

BRED='\033[1;31m'
BGREEN='\033[1;32m'
BBLUE='\033[1;34m'

NC='\033[0m'

OUTFILE="toto"

#./matmul_paper 1024 1024 1024
#./householder 1024 1024

echo -e "${BBLUE}Simple executions${NC}"

if [ -e ./permutation ] ; then
    ./permutation &> $OUTFILE
    RET=$?
    echo -n "Loop permutation  " 
    if [ $RET == 0 ] ; then
	echo -e "                             ${BGREEN}[PASSED]${NC}"
    else
	echo -e "                             ${BRED}[FAILED]${NC}"
    fi
fi

if [ -e ./reverse ] ; then
    ./reverse &> $OUTFILE
    RET=$?
    echo -n "Loop reverse  " 
    if [ $RET == 0 ] ; then
	echo -e "                                 ${BGREEN}[PASSED]${NC}"
    else
	echo -e "                                 ${BRED}[FAILED]${NC}"
    fi
fi

if [ -e ./tile ] ; then
    ./tile &> $OUTFILE
    RET=$?
    echo -n "Loop tiling   " 
    if [ $RET == 0 ] ; then
	echo -e "                                 ${BGREEN}[PASSED]${NC}"
    else
	echo -e "                                 ${BRED}[FAILED]${NC}"
    fi
fi

if [ -e ./unrolling ] ; then
    ./unrolling &> $OUTFILE
    RET=$?
    echo -n "Loop unrolling  " 
    if [ $RET == 0 ] ; then
	echo -e "                               ${BGREEN}[PASSED]${NC}"
    else
	echo -e "                               ${BRED}[FAILED]${NC}"
    fi
fi

echo -e "${BBLUE}Correctness of the result${NC}"

if [ -e ./matmul ] && [ -e matmul_unchanged ] ; then
    ./matmul > tmp
    ./matmul_unchanged > tmp2
    TOTO=`diff tmp tmp2`
    echo -n "Produced code gives the same result:   "
    if [ -z $TOTO ]; then
	echo -e "        ${BGREEN}[PASSED]${NC}"
    else
	echo -e "        ${BRED}[FAILED]${NC}"
    fi
    rm tmp tmp2
fi


rm $OUTFILE
