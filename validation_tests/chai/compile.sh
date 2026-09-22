#!/bin/bash
. ./setup.sh
set -x
#set -e
grep -rEi "CXX_STANDARD|cxx_std_[0-9]+" "${CAMP_ROOT}"/lib*/cmake/camp/*.cmake 2>/dev/null
grep -rEi "CXX_STANDARD|cxx_std_[0-9]+" "${UMPIRE_ROOT}"/lib*/cmake/umpire/*.cmake 2>/dev/null
grep -rEi "CXX_STANDARD|cxx_std_[0-9]+" "${CHAI_ROOT}"/lib*/cmake/chai/*.cmake 2>/dev/null


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
