#!/bin/bash

. ./setup.sh

COMPDEF="-DCMAKE_C_COMPILER=$TEST_CC  -DCMAKE_CXX_COMPILER=$TEST_CXX"


if [ $USECUDA -eq 1 ];then
        echo Using Cuda
        CUDADEF="-DENABLE_CUDA=True"
elif [ $USEROCM -eq 1 ];then
        echo Using Rocm
        COMPDEF="-DCMAKE_C_COMPILER=amdclang -DCMAKE_CXX_COMPILER=hipcc  -DENABLE_HIP=True"
fi


cmake $COMPDEF \
	-DCabana_ROOT=Cabana_ROOT .

make
