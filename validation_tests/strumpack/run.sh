#!/bin/bash -e

export OMP_NUM_THREADS=4

./testPoisson2d 100

mpirun -n 4 ./testPoisson2dMPIDist 100
