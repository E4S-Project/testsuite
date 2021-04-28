#!/bin/bash -e
. ./setup.sh
set -x
#mpirun -np 4 
eval $TEST_RUN ./ex10p --mesh ./beam-quad.mesh

