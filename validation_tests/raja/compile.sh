#!/bin/bash  
. ./setup.sh
#spackLoadUniqueNoX cmake
CUDADEF=""
ROCMDEF=""
set -e
set -x

echo $SPACK_CUDA_ARCH
mkdir build
cd build
if [ $USECUDA -eq 1 ];then
	echo Using Cuda
	CUDADEF="-DENABLE_CUDA=True -DCMAKE_CUDA_ARCHITECTURES=${SPACK_CUDA_ARCH}"
elif [ $USEROCM -eq 1 ];then
        echo Using Rocm
        ROCMDEF="-DCMAKE_C_COMPILER=amdclang -DCMAKE_CXX_COMPILER=hipcc  -DENABLE_HIP=True"
fi

cmake ${ROCMDEF}  ${CUDADEF}  -DRAJA_DIR=${RAJA_ROOT} ..
make
