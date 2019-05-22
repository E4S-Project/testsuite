#!/bin/bash
. ../../setup.sh
spack load openblas
spack load magma
spack load cuda

make c 
