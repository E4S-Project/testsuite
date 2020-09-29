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

rm profile.*
tau_exec  -T serial,clang ./householder 256 256 &> $OUTFILE
RC=$?
echo -n "Execution of C++ instrumented code"
if [ $RC != 0 ]; then
    echo -e "                ${BRED}[FAILED]${NC}"
else
    echo -e "                ${BGREEN}[PASSED]${NC}"
fi
echo -n "Profiled the functions"
if [ `pprof | grep "apply" | wc -l` -gt 0 ] ; then
    echo -e "                            ${BGREEN}[PASSED]${NC}"
else
    echo -e "                            ${BRED}[FAILED]${NC}"
fi
rm $OUTFILE

rm profile.*
tau_exec  -T serial,clang ./mm_c &> $OUTFILE
RC=$?
echo -n "Execution of C instrumented code"
if [ $RC != 0 ]; then
    echo -e "                  ${BRED}[FAILED]${NC}"
else
    echo -e "                  ${BGREEN}[PASSED]${NC}"
fi
echo -n "Profiled the functions"
if [ `pprof | grep "initialize" | wc -l` -gt 0 ] ; then
    echo -e "                            ${BGREEN}[PASSED]${NC}"
else
    echo -e "                            ${BRED}[FAILED]${NC}"
fi
rm $OUTFILE
rm profile.*
