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

echo -e "${BBLUE}Basic tests${NC}"

if [ -e ./basic/arr1 ] ; then
    ./basic/arr1 8 &> $OUTFILE
    RET=$?
    echo -n "arr1:" $EXEC "     "
    if [ $RET == 0 ] ; then
	echo -e "                  ${BGREEN}[PASSED]${NC}"
    else
	echo -e "                  ${BRED}[FAILED]${NC}"
    fi
fi

if [ -e ./basic/arr2 ] ; then
    ./basic/arr2 8 &> $OUTFILE
    RET=$?
    echo -n "arr2:" $EXEC "     "
    if [ $RET == 0 ] ; then
	echo -e "                  ${BGREEN}[PASSED]${NC}"
    else
	echo -e "                  ${BRED}[FAILED]${NC}"
    fi
fi

if [ -e ./basic/basic1 ] ; then
    ./basic/basic1 1 2 4 6 7  &> $OUTFILE
    RET=$?
    echo -n "basic1:" $EXEC "     "
    if [ $RET == 0 ] ; then
	echo -e "                ${BGREEN}[PASSED]${NC}"
    else
	echo -e "                ${BRED}[FAILED]${NC}"
    fi
fi

if [ -e ./basic/basic1 ] ; then
    ./basic/basic2 1 2 4 6 7  &> $OUTFILE
    RET=$?
    echo -n "basic2:" $EXEC "     "
    if [ $RET == 0 ] ; then
	echo -e "                ${BGREEN}[PASSED]${NC}"
    else
	echo -e "                ${BRED}[FAILED]${NC}"
    fi
fi

if [ -e ./basic/basic3 ] ; then
    ./basic/basic3 1 2 4 6 7  &> $OUTFILE
    RET=$?
    echo -n "basic3:" $EXEC "     "
    if [ $RET == 0 ] ; then
	echo -e "                ${BGREEN}[PASSED]${NC}"
    else
	echo -e "                ${BRED}[FAILED]${NC}"
    fi
fi

if [ -e ./basic/basic4 ] ; then
    ./basic/basic4 &> $OUTFILE
    RET=$?
    echo -n "basic4:" $EXEC "     "
    if [ $RET == 0 ] ; then
	echo -e "                ${BGREEN}[PASSED]${NC}"
    else
	echo -e "                ${BRED}[FAILED]${NC}"
    fi
fi

if [ -e ./basic/basic5 ] ; then
    ./basic/basic5 &> $OUTFILE
    RET=$?
    echo -n "basic5:" $EXEC "     "
    if [ $RET == 0 ] ; then
	echo -e "                ${BGREEN}[PASSED]${NC}"
    else
	echo -e "                ${BRED}[FAILED]${NC}"
    fi
fi

if [ -e ./basic/basic6 ] ; then
    ./basic/basic6 &> $OUTFILE
    RET=$?
    echo -n "basic6:" $EXEC "     "
    if [ $RET == 0 ] ; then
	echo -e "                  ${BGREEN}[PASSED]${NC}"
    else
	echo -e "                  ${BRED}[FAILED]${NC}"
    fi
fi

if [ -e ./basic/cct ] ; then
    ./basic/cct &> $OUTFILE
    RET=$?
    echo -n "cct:" $EXEC "     "
    if [ $RET == 0 ] ; then
	echo -e "                   ${BGREEN}[PASSED]${NC}"
    else
	echo -e "                   ${BRED}[FAILED]${NC}"
    fi
fi

if [ -e ./basic/mt1 ] ; then
    ./basic/mt1 &> $OUTFILE
    RET=$?
    echo -n "mt1:" $EXEC "     "
    if [ $RET == 0 ] ; then
	echo -e "                   ${BGREEN}[PASSED]${NC}"
    else
	echo -e "                   ${BRED}[FAILED]${NC}"
    fi
fi

if [ -e ./basic/mt2 ] ; then
    ./basic/mt2 &> $OUTFILE
    RET=$?
    echo -n "mt2:" $EXEC "     "
    if [ $RET == 0 ] ; then
	echo -e "                   ${BGREEN}[PASSED]${NC}"
    else
	echo -e "                   ${BRED}[FAILED]${NC}"
    fi
fi

if [ -e ./basic/pack1 ] ; then
    ./basic/pack1 &> $OUTFILE
    RET=$?
    echo -n "pack1:" $EXEC "     "
    if [ $RET == 0 ] ; then
	echo -e "                 ${BGREEN}[PASSED]${NC}"
    else
	echo -e "                 ${BRED}[FAILED]${NC}"
    fi
fi

if [ -e ./basic/array_size ] ; then
    ./basic/array_size 5 6 &> $OUTFILE
    RET=$?
    echo -n "array_size:" $EXEC "     "
    if [ $RET == 0 ] ; then
	echo -e "            ${BGREEN}[PASSED]${NC}"
    else
	echo -e "            ${BRED}[FAILED]${NC}"
    fi
fi

echo -e "${BBLUE}Matrix factorization${NC}"

rm $OUTFILE
if [ -e ./hh/householder_jit ] ; then
    ./hh/householder_jit 512 512 &> $OUTFILE
    RET=$?
    echo -n "Householder exec:" $EXEC 
    if [ $RET == 0 ] ; then
	echo -e "            ${BGREEN}[PASSED]${NC}"
    else 
	echo -e "            ${BRED}[FAILED]${NC}"
    fi
    RESULT=`grep PASS $OUTFILE`
    echo -n "Householder correct:" $EXEC
    if [ -z $RESULT ]; then
	echo -e "         ${BRED}[FAILED]${NC}"
    else
	echo -e "         ${BGREEN}[PASSED]${NC}"
    fi
	
fi

rm $OUTFILE
