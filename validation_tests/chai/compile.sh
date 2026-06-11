#!/bin/bash
. ./setup.sh
set -x
set -e
mkdir -p build
cd build

if [ $USECUDA -eq 1 ];then
        echo Using Cuda
        CUDADEF="-DCHAI_ENABLE_CUDA=True"
elif [ $USEROCM -eq 1 ];then
        echo Using Rocm
        ROCMDEF="-DCMAKE_C_COMPILER=amdclang -DCMAKE_CXX_COMPILER=hipcc  -DCHAI_ENABLE_HIP=True"
fi


cmake .. ${ROCMDEF} ${CUDADEF}  -Dcamp_DIR=$CAMP_ROOT -Dumpire_DIR=$UMPIRE_ROOT -Dchai_DIR=$CHAI_ROOT #-DBLT_SOURCE_DIR=$BLT_ROOT
make
