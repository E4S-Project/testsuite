#!/bin/bash
. ../../setup.sh
spackLoadUniqueNoX cmake
TESTNAME=axom
export THISDIR=`basename "$PWD"`
if [  "$THISDIR" = "$TESTNAME-cuda" ];then
        spackLoadUnique "$TESTNAME+cuda $TEST_CUDA_ARCH"
	export SPACK_CUDA_ARCH=$(spack find --json /$E4S_TEST_HASH | python3 -c 'import sys,json; print(json.load(sys.stdin)[0]["parameters"]["cuda_arch"][0])')
	export TEST_COMPILER_BACKEND="CUDA"
elif [  "$THISDIR" = "$TESTNAME-rocm" ];then
        spackLoadUnique "$TESTNAME+rocm $TEST_ROCM_ARCH"
	export TEST_COMPILER_BACKEND="HIP"
else
        spackLoadUnique $TESTNAME~cuda~rocm
	export TEST_COMPILER_BACKEND="HOST"
fi

