#!/bin/bash

. ./setup.sh

# barrier-subset.c is only testing MPI
CPROG="dist-work.c  quo-time.c rebind.c trivial.c"
FPROG="quofort.f90"

for c in $CPROG; do
    n=`echo $c | sed "s/.c$//"`
    mpicc -I${QUO_ROOT}/include -I${HWLOC_ROOT}/include $c -o $n -L${QUO_ROOT}/lib -L${HWLOC_ROOT}/lib -lquo -lhwloc -lm
done

for c in $FPROG; do
    n=`echo $c | sed "s/.f90$//"`
    mpifort -I${QUO_ROOT}/lib -I${QUO_ROOT}/include -I${HWLOC_ROOT}/include $c -o $n -L${QUO_ROOT}/lib -L${HWLOC_ROOT}/lib -lquo -lquo-usequo -lhwloc -lm
done
