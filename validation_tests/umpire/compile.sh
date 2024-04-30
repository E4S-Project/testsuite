#!/bin/bash -e
. ./setup.sh
#spack load umpire 
set -x
${TEST_CXX} malloc.cxx  -I$CAMP_ROOT/include   -I$UMPIRE_ROOT/include/ -I$FMT_ROOT/include -L$UMPIRE_ROOT/lib ${UMPIRE_ARCH_ARGS}  -lumpire

#nvcc malloc.cxx -I$CUDA_ROOT/include  -I$CAMP_ROOT/include   -I$UMPIRE_ROOT/include/  -L$UMPIRE_LIB_PATH  -lumpire   -L$CUDA_LIB_PATH -lcuda -lcudart -lcudadevrt 


