#!/bin/bash
. ../../setup.sh

#This test requires a valid Nvidia/CUDA configuration. If run in a container the container must be launched to give access to compatible GPU resources (for docker, depdning on the version, the arguments '--runtime=nvidia' or '--gpus all' may work). The version of CUDA provided by spack and taken as a dependency by MAGMA must match the system's Nvidia driver version. CUDA 10.1 requires Nvidia driver 4.18.39 or higher.

spack load openblas threads=none
spack load magma #^cuda@10.0.130 #Install this spec and uncomment for Nvidia drivers < 4.18.39
spack load cuda  #@10.0.130 #Install this spec and uncomment for Nvidia drivers < 4.18.39

