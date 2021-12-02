#!/bin/bash -e
. ./setup.sh
set -x
cd ./build
export CUDA_MANAGED_FORCE_DEVICE_ALLOC=1
${TEST_RUN} ./MyApp
