#!/bin/bash -e
. ./setup.sh
set -x
export OMP_PROC_BIND=spread 
export OMP_PLACES=threads
export OMP_NUM_THREADS=8
./build/example 500000000 
