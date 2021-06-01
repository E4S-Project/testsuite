#!/bin/bash -e

. ./setup.sh

export OMP_NUM_THREADS=4

./testPoisson2d 100

#mpirun -n 4 
$TEST_RUN ./testPoisson2dMPIDist 100
