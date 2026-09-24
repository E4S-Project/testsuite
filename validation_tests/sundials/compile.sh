#!/bin/bash -e
. ./setup.sh
set -x

if [ "$USECUDA" = "1" ]; then
    EXAMPLE_DIR="cvode/cuda"
elif [ "$USEROCM" = "1" ]; then
    EXAMPLE_DIR="cvode/hip"
else
    EXAMPLE_DIR="cvode/serial"
fi

mkdir -p examples
cp -r "${SUNDIALS_ROOT}/examples/${EXAMPLE_DIR}" examples/
cd "examples/$(basename ${EXAMPLE_DIR})"

CMAKE_EXTRA=()
if [ "$USECUDA" = "1" ]; then
    # Not set anywhere in the generated file; pin to the actual GPU arch
    # rather than relying on CMake's default.
    ARCH_NUM=$(echo "${SPACK_CUDA_ARCH}" | sed 's/sm_//')
    CMAKE_EXTRA+=("-DCMAKE_CUDA_ARCHITECTURES=${ARCH_NUM}")
fi

cmake "${CMAKE_EXTRA[@]}" .
make
