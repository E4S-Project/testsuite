#!/bin/bash

. ./setup.sh

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'

BRED='\033[1;31m'
BGREEN='\033[1;32m'
BBLUE='\033[1;34m'

NC='\033[0m'

OUTFILE=toto

# Basic sanity checks
echo -e "${BBLUE}Sanity checks${NC}"
which clang++ &> $OUTFILE
RET=$?
echo -n "Clang present in the path  " 
if [ $RET == 0 ] ; then
    echo -e "                        ${BGREEN}[PASSED]${NC}"
else
    echo -e "                        ${BRED}[FAILED]${NC}"
    exit -1
fi

clang++ -v &> $OUTFILE
# grepping "clang" would be useless
echo -n "clang++ -v returns something expected  " 
RET=`grep "clang version" $OUTFILE | wc -l`
if [ $RET == 0 ] ; then
    echo -e "            ${BRED}[FAILED]${NC}"
    exit -1
else
    echo -e "            ${BGREEN}[PASSED]${NC}"
fi

echo -e "${BBLUE}Basic tests${NC}"

# clang++ -fjit -o alias alias.cpp
clang++ -stdlib=libc++ -fjit -o basic/arr1 basic/arr1.cpp
RET=$?
echo -n "arr1:" $EXEC "     "
if [ $RET == 0 ] ; then
    echo -e "                  ${BGREEN}[PASSED]${NC}"
else
    echo -e "                  ${BRED}[FAILED]${NC}"
fi

clang++ -stdlib=libc++ -fjit -o basic/arr2 basic/arr2.cpp
RET=$?
echo -n "arr2:" $EXEC "     "
if [ $RET == 0 ] ; then
    echo -e "                  ${BGREEN}[PASSED]${NC}"
else
    echo -e "                  ${BRED}[FAILED]${NC}"
fi

clang++ -stdlib=libc++ -fjit -o basic/basic1 basic/basic1.cpp
RET=$?
echo -n "basic1:" $EXEC "     "
if [ $RET == 0 ] ; then
    echo -e "                ${BGREEN}[PASSED]${NC}"
else
    echo -e "                ${BRED}[FAILED]${NC}"
fi

clang++ -stdlib=libc++ -fjit -o basic/basic2 basic/basic2.cpp
RET=$?
echo -n "basic2:" $EXEC "     "
if [ $RET == 0 ] ; then
    echo -e "                ${BGREEN}[PASSED]${NC}"
else
    echo -e "                ${BRED}[FAILED]${NC}"
fi

clang++ -stdlib=libc++ -fjit -o basic/basic3 basic/basic3.cpp
RET=$?
echo -n "basic3:" $EXEC "     "
if [ $RET == 0 ] ; then
    echo -e "                ${BGREEN}[PASSED]${NC}"
else
    echo -e "                ${BRED}[FAILED]${NC}"
fi

clang++ -stdlib=libc++ -fjit -o basic/basic4 basic/basic4.cpp
RET=$?
echo -n "basic4:" $EXEC "     "
if [ $RET == 0 ] ; then
    echo -e "                ${BGREEN}[PASSED]${NC}"
else
    echo -e "                ${BRED}[FAILED]${NC}"
fi

clang++ -stdlib=libc++ -fjit -o basic/basic5 basic/basic5.cpp
RET=$?
echo -n "basic5:" $EXEC "     "
if [ $RET == 0 ] ; then
    echo -e "                ${BGREEN}[PASSED]${NC}"
else
    echo -e "                ${BRED}[FAILED]${NC}"
fi

#clang++ -stdlib=libc++ -fjit -o basic/basic6 basic/basic6.cpp
#RET=$?
#echo -n "basic6:" $EXEC "     "
#if [ $RET == 0 ] ; then
#    echo -e "                  ${BGREEN}[PASSED]${NC}"
#else
#    echo -e "                  ${BRED}[FAILED]${NC}"
#fi

clang++ -stdlib=libc++ -fjit -o basic/cct basic/cct.cpp
RET=$?
echo -n "cct:" $EXEC "     "
if [ $RET == 0 ] ; then
    echo -e "                  ${BGREEN}[PASSED]${NC}"
else
    echo -e "                  ${BRED}[FAILED]${NC}"
fi

clang++ -stdlib=libc++ -fjit -o basic/mt1 basic/mt1.cpp
RET=$?
echo -n "mt1:" $EXEC "     "
if [ $RET == 0 ] ; then
    echo -e "                  ${BGREEN}[PASSED]${NC}"
else
    echo -e "                  ${BRED}[FAILED]${NC}"
fi

clang++ -stdlib=libc++ -fjit -o basic/mt2 basic/mt2.cpp
RET=$?
echo -n "mt2:" $EXEC "     "
if [ $RET == 0 ] ; then
    echo -e "                  ${BGREEN}[PASSED]${NC}"
else
    echo -e "                  ${BRED}[FAILED]${NC}"
fi

clang++ -stdlib=libc++ -fjit -o basic/pack1 basic/pack1.cpp
RET=$?
echo -n "pack1:" $EXEC "     "
if [ $RET == 0 ] ; then
    echo -e "                ${BGREEN}[PASSED]${NC}"
else
    echo -e "                ${BRED}[FAILED]${NC}"
fi

clang++ -stdlib=libc++ -fjit -o basic/array_size basic/array_size.cpp
RET=$?
echo -n "array_size:" $EXEC "     "
if [ $RET == 0 ] ; then
    echo -e "           ${BGREEN}[PASSED]${NC}"
else
    echo -e "           ${BRED}[FAILED]${NC}"
fi

echo -e "${BBLUE}Matrix factorization${NC}"

clang++ -O3 -stdlib=libc++ -fjit -o hh/householder_jit hh/householder_jit.cpp
RET=$?
echo -n "Householder:" $EXEC "     "
if [ $RET == 0 ] ; then
    echo -e "          ${BGREEN}[PASSED]${NC}"
else
    echo -e "          ${BRED}[FAILED]${NC}"
fi
