#!/bin/bash -ex
. ./setup.sh
#spack load umpire 

#g++ 
${TEST_CXX} malloc.cxx -I$UMPIRE_ROOT/include/ -L$UMPIRE_ROOT/lib  -lumpire 


