#!/bin/bash
. ../../setup.sh

THISDIR=`basename "$PWD"`
if [  "$THISDIR" = "umpire-cuda" ];then
        spackLoadUnique umpire+cuda
	#export UMPIRE_ARCH_ARGS="-I${CUDA_ROOT}/targets/*/include"
	export UMPIRE_ARCH_ARGS="-DENABLE_CUDA=true"
elif [  "$THISDIR" = "umpire-rocm" ];then
        spackLoadUnique "umpire+rocm $TEST_ROCM_ARCH"
	export UMPIRE_ARCH_ARGS=-DENABLE_ROCM=true #"-D__HIP_PLATFORM_AMD__ -I${HIP_ROOT}/include"
        #export ACCEL_DEFINES="-DCMAKE_CXX_COMPILER=hipcc -DCMAKE_C_COMPILER=hipc"
else
        spackLoadUnique umpire~cuda~rocm
fi

