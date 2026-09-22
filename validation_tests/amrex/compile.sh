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
    CMAKE_FLAGS="${CMAKE_FLAGS} -DAMReX_GPU_BACKEND=HIP -DAMReX_AMD_ARCH=${SPACK_ROCM_ARCH} -DCMAKE_CXX_COMPILER=hipcc"
else
    CMAKE_FLAGS="${CMAKE_FLAGS} -DAMReX_GPU_BACKEND=NONE"
fi

cmake ${CMAKE_FLAGS} ..
make -j$(nproc)
cd -

