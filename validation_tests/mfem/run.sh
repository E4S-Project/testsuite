#!/bin/bash

spack load mfem
spack load mpich 

mpirun -np 4 ./ex10p --mesh ./beam-quad.mesh

