#!/bin/bash -ex
. ./setup.sh
#if [ -z ${CC+x} ]; then CC="gcc"; else echo "CC is set to '$CC'"; fi
#cc 
eval $TEST_CC ./hello_world_multi.c -L${QTHREADS_ROOT}/lib -lqthread -I${QTHREADS_ROOT}/include 



