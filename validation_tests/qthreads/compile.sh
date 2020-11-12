#!/bin/bash

. ./setup.sh

set -e
set -x

eval $TEST_CC ./hello_world_multi.c -L${QTHREADS_ROOT}/lib -lqthread -I${QTHREADS_ROOT}/include 
