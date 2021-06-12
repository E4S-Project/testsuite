#!/bin/bash
. ../../setup.sh

spack load cmake
spackLoadUnique tasmanian
#spackLoadUnique openblas threads=openmp

#spack load openblas threads=openmp
#spack load tasmanian@6.0 #+python
#spack load mpich
