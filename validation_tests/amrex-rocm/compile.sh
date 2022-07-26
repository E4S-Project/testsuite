#!/bin/bash -e
. ./setup.sh
set -x

mkdir -p build
cd build

HIPFLAG=""
if [ $USEROCM = 1 ];then
    HIPFLAG="-DAMReX_GPU_BACKEND=HIP -DAMReX_AMD_ARCH=${HCC_AMDGPU_TARGET} -DCMAKE_CXX_COMPILER=hipcc"
fi 

cmake ${HIPFLAG} ..
make -j8
cd -
