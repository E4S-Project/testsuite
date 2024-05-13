#!/bin/bash -e
. ./setup.sh
#spack load openblas threads=none
#spack load superlu 
set -x
./build/c_sample
