#!/bin/bash
. ./setup.sh
gcc -L${QTHREADS_ROOT}/lib -lqthread -I${QTHREADS_ROOT}/include ./hello_world_multi.c



