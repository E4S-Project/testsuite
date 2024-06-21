#!/bin/bash
. ./setup.sh
set -x
set -e
mkdir -p build
cd build
cmake  -DGOTCHA_DIR=$GOTCHA_ROOT ..
make
