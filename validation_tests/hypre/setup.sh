#!/bin/bash
. ../../setup.sh

THISDIR=`basename "$PWD"`
ARCHARGS=""
if [  "$THISDIR" = "hypre-cuda" ];then
        spackLoadUnique "hypre@2.27.0:+cuda $TEST_CUDA_ARCH"
	ARCHARGS="-L${CUDA_LIB_PATH} -Wl,-rpath,${CUDA_LIB_PATH}  -lcudart -lcublas -lcusparse"
	MPIRANKS=1
elif [  "$THISDIR" = "hypre-rocm" ];then
        spackLoadUnique "hypre@2.27.0:+rocm $TEST_ROCM_ARCH"
	#ARCHARGS="-lrocsparse -lrocblas -lamdhip64"
	ARCHARGS="-L${ROCSPARSE_LIB_PATH} -Wl,-rpath,${ROCSPARSE_LIB_PATH} \
                  -L${ROCBLAS_LIB_PATH} -Wl,-rpath,${ROCBLAS_LIB_PATH} \
                  -L${HIP_LIB_PATH} -Wl,-rpath,${HIP_LIB_PATH} \
                  -lrocsparse -lrocblas -lamdhip64"
	MPIRANKS=1
else
        spackLoadUnique hypre@2.27.0:~cuda~rocm
	MPIRANKS=8
fi
