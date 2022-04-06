#!/bin/bash

. ./setup.sh

#RED='\033[0;31m'
#GREEN='\033[0;32m'
#BLUE='\033[0;34m'

#BRED='\033[1;31m'
#BGREEN='\033[1;32m'
#BBLUE='\033[1;34m'

#NC='\033[0m'

#export TAU_MAKEFILE=shared-clang-acc-pdt

export TAU_OPTIONS='-optCompInst -optVerbose'

if [ `arch` == "ppc64le" ]; then
    export TARGET=" -fopenmp-targets=powerpc64le-unknown-linux-gnu"
    export TAU_MAKEFILE=shared-clang-acc-cupti
fi
if [ `arch` == "x86_64" ]; then
    export TAU_MAKEFILE=shared-clang-acc
fi

#clang -O3 -g -fplugin=${LLVM_DIR}/lib/TAU_Profiling.so -mllvm -tau-input-file=./functions_hh.txt -ldl -L${TAU}/lib/$TAU_MAKEFILE -lTAU -Wl,-rpath,${TAU}/lib/$TAU_MAKEFILE householder.c -o householder -lm

clang -fplugin=${LLVM_DIR}/lib/TAU_Profiling.so -mllvm -tau-input-file=./functions_hh.txt -ldl -L${TAU}/lib/$TAU_MAKEFILE -lTAU -Wl,-rpath,${TAU}/lib/$TAU_MAKEFILE  -O3 -g  -fopenacc $TARGET ../householder3.c -o householder3 -lm 
RC=$?
echo -n "Instrumentation"
if [ $RC != 0 ]; then
    echo -e "                               ${BRED}[FAILED]${NC}"
else
    echo -e "                               ${BGREEN}[PASSED]${NC}"
fi
