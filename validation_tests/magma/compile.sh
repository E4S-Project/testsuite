#!/bin/bash
. ../../setup.sh
spack load openblas threads=openmp
spack load magma
spack load cuda

VERBOSE=1 make c 
