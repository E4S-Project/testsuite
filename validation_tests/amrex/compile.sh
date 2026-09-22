#!/bin/bash -e
. ./setup.sh
set -x

#rm -rf build
mkdir -p build
cd build

CMAKE_FLAGS="-DCMAKE_PREFIX_PATH=${AMREX_ROOT}"

if [ "${USECUDA}" = "1" ]; then
    ARCH_NUM=$(echo "${SPACK_CUDA_ARCH}" | sed 's/sm_//')
    CMAKE_FLAGS="${CMAKE_FLAGS} -DAMReX_GPU_BACKEND=CUDA -DCMAKE_CUDA_ARCHITECTURES=${ARCH_NUM}"
elif [ "${USEROCM}" = "1" ]; then
    if [ -n "${ROCM_PATH}" ]; then
        ROCM_PREFIX="${ROCM_PATH}"
    elif command -v hipcc >/dev/null 2>&1; then
        ROCM_PREFIX=$(dirname "$(dirname "$(readlink -f "$(command -v hipcc)")")")
    else
        echo "ERROR: cannot determine ROCm prefix (ROCM_PATH unset, hipcc not on PATH)" >&2
        exit 1
    fi
    CMAKE_FLAGS="-DCMAKE_PREFIX_PATH=${AMREX_ROOT};${ROCM_PREFIX} -DAMReX_GPU_BACKEND=HIP -DCMAKE_CXX_COMPILER=hipcc"
else
    CMAKE_FLAGS="${CMAKE_FLAGS} -DAMReX_GPU_BACKEND=NONE"
fi

cmake ${CMAKE_FLAGS} ..
make -j$(nproc)
cd -
