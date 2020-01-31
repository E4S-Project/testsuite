#!/bin/bash -ex
. ./setup.sh
gcc ./hello_world_multi.c -L${QTHREADS_ROOT}/lib -lqthread -I${QTHREADS_ROOT}/include 



