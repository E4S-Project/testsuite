#!/bin/bash

. ./setup.sh

#RED='\033[0;31m'
#GREEN='\033[0;32m'
#BLUE='\033[0;34m'

#BRED='\033[1;31m'
#BGREEN='\033[1;32m'
#BBLUE='\033[1;34m'

#NC='\033[0m'

# Don't do this for the quick test version

function foo() {
    echo -e "\033[01;34m[Backend: gfortran]\e[0m"
    export F18_FC=gfortran
    
    cd lapack && make blas_testing lapack_testing TIMER=INT_ETIME
    cd ..
    cd gfortran.dg/build && ctest -j -R CMP && ctest -j -R EXE
    cd ..
    
    echo -e "\033[01;34m[Backend: clang]\e[0m"
    export F18_FC=clang
    
    cd lapack && make clean && make blas_testing lapack_testing
    cd ..
    cd gfortran.dg/build && ctest -j -R CMP && ctest -j -R EXE  TIMER=INT_CPU_TIME
    cd ..
}

for d in libraries   modules  simple ; do
    cd $d
    ./run.sh
    cd ..
done

./version 
RC=$?
echo -n "Version number"
if [ $RC != 0 ]; then
    echo -e "                    ${BRED}[FAILED]${NC}"
else
    echo -e "                    ${BGREEN}[PASSED]${NC}"
fi
