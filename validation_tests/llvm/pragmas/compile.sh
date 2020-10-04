#!/bin/bash

. ./setup.sh
#clang -O3 -o blocklu blocklu.c -lrt -lm

OUTFILE=toto

# Basic sanity checks
echo -e "${BBLUE}Sanity checks${NC}"
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

cd loop
./compile.sh
cd ..



