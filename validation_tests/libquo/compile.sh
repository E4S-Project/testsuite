#!/bin/bash

. ./setup.sh
set -e
set -x
# barrier-subset.c is only testing MPI
CPROG="dist-work.c  quo-time.c rebind.c trivial.c"
FPROG="quofort.f90"

for c in $CPROG; do
    n=`echo $c | sed "s/.c$//"`
    #mpicc 
    eval $TEST_CC_MPI -I${LIBQUO_ROOT}/include -I${HWLOC_ROOT}/include $c -o $n -L${LIBQUO_ROOT}/lib -L${HWLOC_ROOT}/lib -lquo -lhwloc -lm
done

for c in $FPROG; do
    n=`echo $c | sed "s/.f90$//"`
    #mpifort 
    eval $TEST_FTN_MPI -I${LIBQUO_ROOT}/lib -I${LIBQUO_ROOT}/include -I${HWLOC_ROOT}/include $c -o $n -L${LIBQUO_ROOT}/lib -L${HWLOC_ROOT}/lib -lquo -lquo-usequo -lhwloc -lm
done
