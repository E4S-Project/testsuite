#!/bin/bash -e
. ./setup.sh
#spack load umpire 
set -x
#${TEST_CXX} malloc.cxx ${UMPIRE_ARCH_ARGS}  -I$CAMP_ROOT/include   -I$UMPIRE_ROOT/include/ -I$FMT_ROOT/include -L$UMPIRE_ROOT/lib   -lumpire
#${TEST_CXX} malloc.cxx ${UMPIRE_ARCH_ARGS} -I$UMPIRE_ROOT/include/ -L$UMPIRE_ROOT/lib   -lumpire
#nvcc malloc.cxx -I$CUDA_ROOT/include  -I$CAMP_ROOT/include   -I$UMPIRE_ROOT/include/  -L$UMPIRE_LIB_PATH  -lumpire   -L$CUDA_LIB_PATH -lcuda -lcudart -lcudadevrt 
mkdir -p build
cd build
cmake  ${UMPIRE_ARCH_ARGS}  -Dumpire_DIR=$UMPIRE_ROOT -Dcamp_DIR=$CAMP_ROOT  ..
VERBOSE=1 make
