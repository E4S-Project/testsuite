#!/bin/bash -e

. ./setup.sh
set -x
export OMP_NUM_THREADS=1

./testPoisson2d 100 $runArg 

#mpirun -n 4 
$TEST_RUN ./testPoisson2dMPIDist 100 $runArg 
