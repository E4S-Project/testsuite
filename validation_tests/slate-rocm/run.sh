#!/bin/bash

. ./setup.sh

#eval $TEST_RUN 
srun -n 4 ./slate04_blas
