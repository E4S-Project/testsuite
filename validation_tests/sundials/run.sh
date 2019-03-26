#!/bin/bash

spack load sundials 
spack load mpich 

mpirun -np 4 examples/nvector/parallel/test_nvector_mpi
