#!/bin/bash

#. ../setup.sh
OMP_NUM_THREADS=4 mpiexec -n 4 ./pddrive -r 2 -c 2 g20.rua
