#!/bin/bash -e
. ./setup.sh
set -x
export OMP_NUM_THREADS=1
pymolcas 005.input -f
grep -e "Happy landing!" 005.log
