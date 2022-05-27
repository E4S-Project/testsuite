#!/bin/bash 

. ./setup.sh
set -x
set -e
cd build
cmake CMakeLists.txt \
	-DCMAKE_C_COMPILER=$TEST_CC \
	-DCMAKE_CXX_COMPILER=$TEST_CXX \
	-DLegion_DIR=${LEGION_ROOT}/share/Legion/cmake

make
cd ../

