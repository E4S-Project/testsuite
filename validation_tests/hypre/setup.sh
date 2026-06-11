#!/bin/bash
. ../../setup.sh

THISDIR=`basename "$PWD"`
ARCHARGS=""
if [  "$THISDIR" = "hypre-cuda" ];then
        spackLoadUnique "hypre@2.27.0:+cuda $TEST_CUDA_ARCH"
	ARCHARGS="-lcudart -lcublas -lcusparse"
elif [  "$THISDIR" = "hypre-rocm" ];then
        spackLoadUnique "hypre@2.27.0:+rocm $TEST_ROCM_ARCH"
	ARCHARGS="-lrocsparse -lrocblas -lamdhip64"
else
        spackLoadUnique hypre@2.27.0:~cuda~rocm
fi
