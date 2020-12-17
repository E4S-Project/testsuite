#!/bin/bash

. ./setup.sh

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'

BRED='\033[1;31m'
BGREEN='\033[1;32m'
BBLUE='\033[1;34m'

NC='\033[0m'

#export TAU_MAKEFILE=shared-clang-pdt
export TAU_MAKEFILE=shared-TEST-clang
#export TAU_OPTIONS='-optCompInst -optVerbose'

export LLVM_DIR=/home/users/fdeny/llvm_build/pluginVersions/plugin-tau-llvm-module-11/install

ERRFILE="toto"

#which clang
#echo $LLVM_DIR

echo -e "${BBLUE}Instrumentation${NC}"

clang++ -c -O3 -g -fplugin=${LLVM_DIR}/lib/TAU_Profiling_CXX.so -mllvm -tau-input-file=./functions_CXX_hh.txt householder.cpp &> $ERRFILE
clang++ -o householder householder.o -fplugin=${LLVM_DIR}/lib/TAU_Profiling_CXX.so -ldl -L${TAU}/lib/$TAU_MAKEFILE -lTAU -Wl,-rpath,${TAU}/lib/$TAU_MAKEFILE 
RC=$?
echo -n "C++ instrumentation"
if [ $RC != 0 ]; then
    echo -e "                               ${BRED}[FAILED]${NC}"
else
    echo -e "                               ${BGREEN}[PASSED]${NC}"
fi
echo -n "Instrumented functions"
if [ `grep "Instrument"  $ERRFILE | wc -l` -gt 0 ] ; then
    echo -e "                            ${BGREEN}[PASSED]${NC}"
else
    echo -e "                            ${BRED}[FAILED]${NC}"
fi
rm $ERRFILE $ERRFILE2


