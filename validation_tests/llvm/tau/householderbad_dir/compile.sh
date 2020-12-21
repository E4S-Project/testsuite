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


# This one is expected to give an error
clang++ -c -O3 -g -fplugin=${LLVM_DIR}/lib/TAU_Profiling_CXX.so -mllvm -tau-input-file=./functions_CXX_hh_bad.txt householder_bad.cpp &> $ERRFILE
clang++ -fplugin=${LLVM_DIR}/lib/TAU_Profiling_CXX.so -ldl -L${TAU}/lib/$TAU_MAKEFILE -lTAU -Wl,-rpath,${TAU}/lib/$TAU_MAKEFILE householder_bad.o -o householder-bad  
RC=$?
echo -n "Error when the input file is wrong"
if [ $RC != 0 ] ; then
    echo -e "                   ${BRED}[FAILED]${NC} compilation failed"
else
    if [ `grep "Wrong syntax:"  $ERRFILE | wc -l` -gt 0 ] ; then
	echo -e "                ${BGREEN}[PASSED]${NC}"
    else
	echo -e "                ${BRED}[FAILED]${NC} compiled but no warning generated"
    fi
fi
rm $ERRFILE 
rm householder-bad


