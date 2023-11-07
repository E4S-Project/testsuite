#!/bin/bash
. ./setup.sh
set -x
set -e
mkdir -p build
cd build

if [ $USECUDA -eq 1 ];then
        echo Using Cuda
        CUDADEF="-DENABLE_CUDA=True"
elif [ $USEROCM -eq 1 ];then
        echo Using Rocm
        ROCMDEF="-DCMAKE_C_COMPILER=amdclang -DCMAKE_CXX_COMPILER=hipcc  -DENABLE_HIP=True"
fi


cmake .. ${ROCMDEF}  -DBLT_SOURCE_DIR=$BLT_ROOT -Dcamp_DIR=$CAMP_ROOT -Dumpire_DIR=$UMPIRE_ROOT -Dchai_DIR=$CHAI_ROOT
make
