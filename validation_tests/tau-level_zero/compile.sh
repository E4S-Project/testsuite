#!/bin/bash
. ./setup.sh
set -x
set -e
mkdir  ./ze_gemm/build
cd ./ze_gemm/build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
