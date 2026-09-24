#!/bin/bash
source ./setup.sh
set -x
set -e

# ADIOS2 C-API install test (extracted from upstream
# testing/install/C) 
CMAKE_EXTRA_ARGS=()
if command -v "${TEST_RUN_CMD}" >/dev/null 2>&1; then
	CMAKE_EXTRA_ARGS+=("-DMPIEXEC_EXECUTABLE=$(command -v ${TEST_RUN_CMD})")
fi

mkdir -p build
cd build
cmake -DCMAKE_PREFIX_PATH="${ADIOS2_ROOT}" \
      -DCMAKE_C_COMPILER="${TEST_CC_MPI}" \
      -DCMAKE_CXX_COMPILER="${TEST_CXX_MPI}" \
      "${CMAKE_EXTRA_ARGS[@]}" \
      ../src
cmake --build .
