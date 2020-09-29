#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'

BRED='\033[1;31m'
BGREEN='\033[1;32m'
BBLUE='\033[1;34m'

NC='\033[0m'

clang -fopenacc -o basic basic.c
clang -fopenacc -o basic2 basic2.c
clang -fopenacc -o jacobi jacobi.c

if [ `arch` == "ppc64le" ]; then
    ARCHITECTURES=(nvptx64-nvidia-cuda powerpc64le-unknown-linux-gnu)
fi
if [ `arch` == "x86_64" ]; then
#    ARCHITECTURES=(nvptx64-nvidia-cuda x86_64-unknown-linux-gnu)
    ARCHITECTURES=(x86_64-unknown-linux-gnu)
fi
for TARGET in ${ARCHITECTURES[@]} ; do
    clang -O3 -fopenacc -fopenmp-targets=$TARGET -o jacobi_off_$TARGET jacobi.c
done

clang -fPIC -shared -fopenacc -o profiling.so profiling.c
RC=$?
echo -n "Mini profiling library"
if [ $RC != 0 ]; then
    echo -e "                            ${BRED}[FAILED]${NC}"
else
    echo -e "                            ${BGREEN}[PASSED]${NC}"
fi
