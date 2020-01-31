#!/bin/bash -ex
. ./setup.sh

echo "Arrays Write Example"
mpirun -np 8 ./arrays_write
ls -lh arrays.bp
echo "Arrays Read Example"
mpirun -np 8 ./arrays_read arrays.bp

