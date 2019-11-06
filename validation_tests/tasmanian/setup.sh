#!/bin/bash
. ../../setup.sh

spackLoadUnique tasmanian@6.0
spackLoadUnique openblas threads=openmp

#spack load openblas threads=openmp
#spack load tasmanian@6.0 #+python
#spack load mpich
