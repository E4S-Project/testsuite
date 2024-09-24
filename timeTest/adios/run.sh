#!/bin/bash -e
. ./setup.sh
set -x
echo "Arrays Write Example"
#mpirun -np 8 
eval $TEST_RUN ./arrays_write
ls -lh arrays.bp
echo "Arrays Read Example"
#mpirun -np 8 
eval $TEST_RUN ./arrays_read arrays.bp

