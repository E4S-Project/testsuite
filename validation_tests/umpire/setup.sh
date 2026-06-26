#!/bin/bash
. ../../setup.sh

THISDIR=`basename "$PWD"`
if [  "$THISDIR" = "umpire-cuda" ];then
        spackLoadUnique umpire+cuda
	#export UMPIRE_ARCH_ARGS="-I${CUDA_ROOT}/targets/*/include"
	export SPACK_CUDA_ARCH=$(spack find --json /$E4S_TEST_HASH | python3 -c 'import sys,json; print(json.load(sys.stdin)[0]["parameters"]["cuda_arch"][0])')
	export UMPIRE_ARCH_ARGS="-DUMPIRE_ENABLE_CUDA=true"
elif [  "$THISDIR" = "umpire-rocm" ];then
        spackLoadUnique "umpire+rocm $TEST_ROCM_ARCH"
	export UMPIRE_ARCH_ARGS=-DUMPIRE_ENABLE_HIP=true # We may need this:  -DCMAKE_C_COMPILER=amdclang -DCMAKE_CXX_COMPILER=hipcc   #  Not sure about this: "-D__HIP_PLATFORM_AMD__ -I${HIP_ROOT}/include"
        #export ACCEL_DEFINES="-DCMAKE_CXX_COMPILER=hipcc -DCMAKE_C_COMPILER=hipc"
else
        spackLoadUnique umpire~cuda~rocm
fi

