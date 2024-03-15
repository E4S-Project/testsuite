#!/bin/bash  
. ./setup.sh
spackLoadUniqueNoX cmake
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
        ROCMDEF="-DCMAKE_C_COMPILER=amdclang -DCMAKE_CXX_COMPILER=hipcc  -DENABLE_HIP=True"
fi

cmake ${ROCMDEF}  ${CUDADEF} -DBLT_SOURCE_DIR=${BLT_ROOT} -DRAJA_DIR=${RAJA_ROOT} ..
make
