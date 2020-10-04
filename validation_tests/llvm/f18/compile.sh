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

# Don't do this for the quick test version

function foo() {
    echo -e "\033[01;34m[Backend: gfortran]\e[0m"
    export F18_FC=gfortran
    
    ulimit -s unlimited
    cd lapack && make blaslib lapacklib TIMER=INT_ETIME
    cd ..
    
    cd gfortran.dg && if [ ! -d build ] ; then mkdir build ; fi && cd build && cmake .. -DFC=f18
    cd ../..
    
    echo -e "\033[01;34m[Backend: clang]\e[0m"
    export F18_FC=clang
    cd lapack && make clean && make blaslib lapacklib TIMER=INT_CPU_TIME
    cd ..
    
    cd gfortran.dg && if [ ! -d build ] ; then mkdir build ; fi && cd build && cmake .. -DFC=f18
    cd ../..
}

echo -e "${BBLUE}Sanity checks (presence) ${NC}"
which f18 &> $OUTFILE
RET=$?
echo -n "f18 present in the path  " 
if [ $RET == 0 ] ; then
    echo -e "                          ${BGREEN}[PASSED]${NC}"
else
    echo -e "                          ${BRED}[FAILED]${NC}"
    exit -1
fi

f18 -v &> $OUTFILE
# grepping "f18" would be useless
echo -n "f18 -v returns something expected  "
RET=`grep "f18 compiler" $OUTFILE | wc -l`
if [ $RET == 0 ] ; then
    echo -e "                ${BRED}[FAILED]${NC}"
    exit -1
else
    echo -e "                ${BGREEN}[PASSED]${NC}"
fi




for d in libraries   modules  simple ; do
    cd $d
    ./compile.sh
    cd ..
done

f18 -o version version.f90 
