#!/bin/bash
. ./setup.sh
#mpicxx
set -x
eval $TEST_CXX_MPI -o castle main.cpp -I${OMEGA_H_ROOT}/include -L${OMEGA_H_LIB_PATH}  -lomega_h
