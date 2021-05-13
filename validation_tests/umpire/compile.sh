#!/bin/bash -e
. ./setup.sh
#spack load umpire 
set -x
#g++ 
${TEST_CXX} malloc.cxx  -I$CAMP_ROOT/include   -I$UMPIRE_ROOT/include/ -L$UMPIRE_ROOT/lib  -lumpire 


