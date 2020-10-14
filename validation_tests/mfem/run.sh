#!/bin/bash -ex
. ./setup.sh
#mpirun -np 4 
eval $TEST_RUN ./ex10p --mesh ./beam-quad.mesh

