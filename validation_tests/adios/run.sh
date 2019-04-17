#!/bin/bash -e
. ../../setup.sh
spack load adios
spack load lz4
spack load zfp
spack load sz
spack load zlib
spack load c-blosc
spack load mpich



echo "Arrays Write Example"
mpirun -np 8 ./arrays_write
ls -lh arrays.bp
echo "Arrays Read Example"
mpirun -np 8 ./arrays_read arrays.bp

