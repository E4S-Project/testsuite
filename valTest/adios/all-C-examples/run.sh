#!/bin/bash

spack load adios
spack load lz4
spack load zfp
spack load sz
spack load zlib
spack load c-blosc
spack load mpich


cd adios-1.13.1/examples/C/arrays

echo "Arrays Write Example"
mpirun -np 8 ./arrays_write
ls -lh arrays.bp
echo "Arrays Read Example"
mpirun -np 8 ./arrays_read arrays.bp

