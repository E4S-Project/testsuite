#!/bin/bash

spack load adios
spack load lz4
spack load zfp
spack load sz
spack load zlib
spack load mpich

export LD_LIBRARY_PATH=`pwd`/c-blosc-install/lib:$LD_LIBRARY_PATH

cd adios-1.13.1/examples/C/arrays

echo "Arrays Write Example"
mpirun -np 8 ./arrays_write
ls -lh arrays.bp
echo "Arrays Read Example"
mpirun -np 8 ./arrays_read arrays.bp

