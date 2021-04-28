#!/bin/bash -e
. ./setup.sh
#spack load umpire 
set -x
#g++ 
${TEST_CXX} malloc.cxx -I$UMPIRE_ROOT/include/ -L$UMPIRE_ROOT/lib  -lumpire 


