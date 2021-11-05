#!/bin/bash  
. ./setup.sh
spackLoadUnique cmake
spackLoadUnique blt
CUDADEF=""
ROCMDEF=""
set -e
set -x
mkdir build
cd build
if [ $USECUDA -eq 1 ];then
	echo Using Cuda
	CUDADEF="-DENABLE_CUDA=True"
elif [ $USEROCM -eq 1 ];then
        echo Using Rocm
        ROCMDEF="-DENABLE_HIP=True"
fi

cmake ${ROCMDEF}  ${CUDADEF} -DBLT_SOURCE_DIR=${BLT_ROOT} -DRAJA_DIR=${RAJA_ROOT} ..
make
