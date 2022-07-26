#!/bin/bash -e
. ./setup.sh
set -x

mkdir -p build
cd build

CUDAFLAG=""
if [ $USECUDA = 1 ];then
    CUDAFLAG="-DAMReX_GPU_BACKEND=CUDA"
fi 

cmake ${CUDAFLAG} ..
make -j8
cd -
