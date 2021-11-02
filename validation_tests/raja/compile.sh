#!/bin/bash -e 
. ./setup.sh
spackLoadUnique cmake
spackLoadUnique blt

USECUDA=""
spack find -v raja | grep +cuda
res=$?
set -e
set -x
#if command -v CC &> /dev/null
#then
#	export CXX=CC
#fi
#echo "CXX is ${CXX}"
mkdir build
cd build
if [ $res -eq 0 ];then
	echo Using Cuda
	USECUDA="-DENABLE_CUDA=True"
fi
cmake ${USECUDA} -DBLT_SOURCE_DIR=${BLT_ROOT} -DRAJA_DIR=${RAJA_ROOT} ..
make
