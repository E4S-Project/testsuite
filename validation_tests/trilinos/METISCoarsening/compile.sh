#!/bin/bash
spack load mpich 
spack load cmake 
spack load trilinos
export TRILINOS=`spack location -i trilinos`
make clean
make 

