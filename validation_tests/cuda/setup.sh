#!/bin/bash

. ../../setup.sh

spackLoadUnique cuda
cuda_ret=$?
export CUDA_PATH=$CUDA_ROOT
return $cuda_ret
