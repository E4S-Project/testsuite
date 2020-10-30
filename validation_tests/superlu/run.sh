#!/bin/bash -e
. ./setup.sh
#spack load openblas threads=none
#spack load superlu 
-x
./c_sample
