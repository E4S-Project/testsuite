#!/bin/bash -e

. ./setup.sh

cmake . \
  -DCMAKE_C_COMPILER=$TEST_CC \
  -DCMAKE_CXX_COMPILER=$TEST_CXX 

make -j2
