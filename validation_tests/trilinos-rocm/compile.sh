#!/bin/bash -e 
. ./setup.sh
set -x

mkdir -p build
cd build

if [ $USEROCM -eq 1 ];then
        echo Using ROCm
        ROCMDEF="-DENABLE_E4S_ROCM=True"
fi

cmake ${ROCMDEF} ..
make VERBOSE=1
cd -
