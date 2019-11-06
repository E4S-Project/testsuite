#!/bin/bash
. ./setup.sh
mpirun -np 4 ./ex10p --mesh ./beam-quad.mesh

