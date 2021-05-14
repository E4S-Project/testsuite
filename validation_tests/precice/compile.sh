#!/bin/bash
. ./setup.sh
#gcc
set -x
eval $TEST_CC  solverdummy.c -I${PRECICE_ROOT}/include -L${PRECICE_LIB_PATH}  -lprecice -o solverdummy
