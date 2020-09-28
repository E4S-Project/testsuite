#!/bin/bash

. ./setup.sh


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

for d in libraries   modules  simple ; do
    cd $d
    ./compile.sh
    cd ..
done

f18 -o version version.f90 
