#!/bin/bash
. ../../setup.sh
spack load mfem 
spack load mpich

make  
