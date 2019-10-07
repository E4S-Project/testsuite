#!/bin/bash
. ./setup.sh
OMP_NUM_THREADS=4 mpirun -n 4 ./ie2d
