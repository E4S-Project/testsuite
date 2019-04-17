#!/bin/bash -e
. ../../setup.sh
spack load adios
spack load lz4
spack load zfp
spack load sz
spack load zlib
spack load c-blosc
spack load mpich

make
