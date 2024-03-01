#!/bin/bash -e 
. ./setup.sh
set -x

mkdir -p build
cd build

CUDADEF=""
if [ $USECUDA -eq 1 ];then
        echo Using Cuda
        CUDADEF="-DENABLE_E4S_CUDA=True"
fi

cmake  ${CUDADEF}  ..
make
cd -
