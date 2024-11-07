#!/bin/bash

. ../../setup.sh

spackLoadUnique cuda
cuda_ret=$?
export CUDA_PATH=$CUDA_ROOT
exit $cuda_ret
