#!/bin/bash
. ./setup.sh
#gcc 
eval $TEST_CC  solverdummy.c -I${PRECICE_ROOT}/include -L${PRECICE_LIB_DIR}  -lprecice -o solverdummy
