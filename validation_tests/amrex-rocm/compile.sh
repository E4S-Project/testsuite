#!/bin/bash -e
. ./setup.sh
set -x

printenv

git clone https://github.com/AMReX-Codes/amrex-tutorials.git
mv amrex-tutorials/ExampleCodes/* .

mkdir -p build
cd build

HIPFLAG=""
if [ $USEROCM = 1 ];then
    HIPFLAG="-DAMReX_GPU_BACKEND=HIP -DAMReX_AMD_ARCH=${HCC_AMDGPU_TARGET} -DCMAKE_CXX_COMPILER=hipcc"
fi 

cmake -DAMReX_ROOT=${AMREX_ROOT} ${HIPFLAG} ..
make -j16
cd -
