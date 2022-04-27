#!/bin/bash -e
. ./setup.sh
set -x

git clone https://github.com/AMReX-Codes/amrex-tutorials.git
mv amrex-tutorials/ExampleCodes/* .

mkdir -p build
cd build

CUDAFLAG=""
if [ $USECUDA = 1 ];then
    CUDAFLAG="-DAMReX_GPU_BACKEND=CUDA"
fi 

cmake -DAMReX_ROOT=${AMREX_ROOT} ${CUDAFLAG} ..
make -j16
cd -
