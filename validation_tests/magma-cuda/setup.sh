#!/bin/bash
. ../../setup.sh

#This test requires a valid Nvidia/CUDA configuration. If run in a container the container must be launched to give access to compatible GPU resources (for docker, depdning on the version, the arguments '--runtime=nvidia' or '--gpus all' may work). The version of CUDA provided by spack and taken as a dependency by MAGMA must match the system's Nvidia driver version. CUDA 10.1 requires Nvidia driver 4.18.39 or higher.
#NEW: ROCm configurations should now be supported, though this is experimental and requires further testing.

#spack load openblas threads=none
#spackLoadUnique magma #^cuda@10.0.130 #Install this spec and uncomment for Nvidia drivers < 4.18.39
#spackLoadUnique openblas threads=none
#spack load cuda  #@10.0.130 #Install this spec and uncomment for Nvidia drivers < 4.18.39

THISDIR=`basename "$PWD"`
if [  "$THISDIR" = "magma-cuda"  ];then
	spackLoadUnique "magma+cuda $TEST_CUDA_ARCH"
	export ACCEL_LIBS="-L$CUDA_ROOT/lib64 -lcublas -lcudart -lcusparse"
	export ACCEL_INC="-I$CUDA_ROOT/include"
	export E4S_ACCEL="-DE4S_CUDA"
	export HIP_PLATFORM="-D__HIP_PLATFORM_NVIDIA__"
else [  "$THISDIR" = "magma-rocm" ];
	spackLoadUnique "magma $TEST_ROCM_ARCH"
	export ACCEL_LIBS="-L$HIP_ROOT/lib -lamdhip64 -lhiprtc-builtins"
	export ACCEL_INC="-I$HIP_ROOT/include -I$HIPBLAS_ROOT/include -I$HIPSPARSE_ROOT/include"
	export E4S_ACCEL="-DE4S_ROCM"
	export HIP_PLATFORM="-D__HIP_PLATFORM_AMD__"
fi
