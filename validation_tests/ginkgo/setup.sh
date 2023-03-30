#!/bin/bash
. ../../setup.sh

THISDIR=`basename "$PWD"`
export GINKGO_DIRS=$(ls -d */ | grep -v cuda |  grep -v par-ilu-convergence | grep -v preconditioner-export | grep -v nine-pt-stencil-solver )
if [  "$THISDIR" = "ginkgo-cuda" ];then
        spackLoadUnique "ginkgo+cuda $TEST_CUDA_ARCH"
	export GINKGO_DIRS=$(ls -d */ | grep -v par-ilu-convergence | grep -v preconditioner-export | grep -v nine-pt-stencil-solver )
elif [  "$THISDIR" = "ginkgo-rocm" ];then
        spackLoadUnique "ginkgo+rocm $TEST_ROCM_ARCH"
        #export ACCEL_DEFINES="-DCMAKE_CXX_COMPILER=hipcc"
else
        spackLoadUnique ginkgo~cuda~rocm
fi

