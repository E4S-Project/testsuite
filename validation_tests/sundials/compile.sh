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

# --- CMake >= 3.27 compatibility for CUDA ---
if [ "$USECUDA" = "1" ]; then
    sed -i 's/find_package(CUDA REQUIRED)/find_package(CUDAToolkit REQUIRED)/g' CMakeLists.txt
    sed -i 's/${CUDA_cusolver_LIBRARY}/CUDA::cusolver/g' CMakeLists.txt
    sed -i 's/${CUDA_cusparse_LIBRARY}/CUDA::cusparse/g' CMakeLists.txt
fi

# Fix missing mpi.h for nvcc (CUDA) and hipcc (ROCm) ---
# Extract the MPI prefix exactly as CMake sees it to bypass FindMPI stripping
MPICC_PATH=$(grep -m 1 'bin/mpicc' CMakeLists.txt | cut -d '"' -f 2)
if [ -n "$MPICC_PATH" ]; then
    MPI_DIR=$(dirname $(dirname "$MPICC_PATH"))
    # Explicitly force the MPI include directory onto all targets (device code needs this)
    sed -i "/project(/a include_directories(\"${MPI_DIR}/include\")" CMakeLists.txt
fi

# Inject FindMPI to resolve the linking step correctly
sed -i '/project(/a find_package(MPI REQUIRED)' CMakeLists.txt
sed -i 's/target_link_libraries(${example} ${SUNDIALS_LIBRARIES})/target_link_libraries(${example} ${SUNDIALS_LIBRARIES} MPI::MPI_C)/g' CMakeLists.txt

CMAKE_EXTRA=()
if [ "$USECUDA" = "1" ]; then
    # Not set anywhere in the generated file; pin to the actual GPU arch
    # rather than relying on CMake's default.
    ARCH_NUM=$(echo "${SPACK_CUDA_ARCH}" | sed 's/sm_//')
    CMAKE_EXTRA+=("-DCMAKE_CUDA_ARCHITECTURES=${ARCH_NUM}")
fi

cmake "${CMAKE_EXTRA[@]}" .
make
