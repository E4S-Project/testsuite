#!/bin/bash -e
. ./setup.sh
set -x
cd ./build
export CUDA_MANAGED_FORCE_DEVICE_ALLOC=1
export CUDA_LAUNCH_BLOCKING=1
export OMP_NUM_THREADS=4
${TEST_RUN} ./MueLU --its=100 --matrixType=Laplace2D --nx=632 --nx=632 --stacked-timer --xml=../scaling.xml
#./iallreduce 
#./Zoltan 1024
