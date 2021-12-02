#!/bin/bash -e
. ./setup.sh
set -x
cd ./build
export CUDA_MANAGED_FORCE_DEVICE_ALLOC=1
export OMP_NUM_THREADS=4
${TEST_RUN} ./Zoltan
