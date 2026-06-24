#!/bin/bash
. ../../setup.sh

spackLoadUniqueNoX cmake

THISDIR=`basename "$PWD"`
USECUDA=0
USEROCM=0
if [  "$THISDIR" = "raja-cuda" ];then
	spackLoadUnique raja+cuda
	export SPACK_CUDA_ARCH=$(spack find --json /$RAJA_HASH | python3 -c 'import sys,json; print(json.load(sys.stdin)[0]["parameters"]["cuda_arch"][0])')
	USECUDA=1
elif [  "$THISDIR" = "raja-rocm" ];then
	spackLoadUnique raja+rocm
	USEROCM=1
else
	spackLoadUnique raja~cuda~rocm
fi
