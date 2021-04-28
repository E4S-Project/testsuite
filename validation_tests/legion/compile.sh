#!/bin/bash -e

. ./setup.sh
set -x
cd build
cmake CMakeLists.txt \
	-DCMAKE_C_COMPILER=$TEST_CC \
	-DCMAKE_CXX_COMPILER=$TEST_CXX \
	-DLegion_DIR=${LEGION_ROOT}/share/Legion/cmake

make
cd ../

