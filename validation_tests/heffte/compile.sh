#!/bin/bash -e

. ./setup.sh



if [ -z "$HEFFTE_ROOT" ]; then
    echo "Error: heffte is not loaded or could not be found."
    exit 1
fi

# Copy the testing directory from heffte to our local workspace
cp -r "${HEFFTE_ROOT}/share/heffte/testing" .

# Fix potential placeholder paths or hardcoded relative paths in CMakeLists.txt
# and point directly to the examples directory in the heffte installation.
sed -i -E "s|add_subdirectory\([^)]+\)|add_subdirectory(\"${HEFFTE_ROOT}/share/heffte/examples\" examples)|" testing/CMakeLists.txt

mkdir -p build
cd build

# Base CMake options. We define both ROOT and DIR to cover older and newer conventions.
CMAKE_OPTS=(
    "-DHeffte_ROOT=${HEFFTE_ROOT}"
    "-DHeffte_DIR=${HEFFTE_ROOT}/lib/cmake/Heffte"
)

# Provide MPI_HOME based on the loaded mpiexec
if command -v mpiexec >/dev/null 2>&1; then
    MPI_HOME=$(dirname $(dirname $(which mpiexec)))
    CMAKE_OPTS+=("-DMPI_HOME=${MPI_HOME}")
fi

# Check if ROCm was enabled, and if so add the necessary CMake flags
# for HIP, LLVM, COMGR, and ROCFFT (mirroring the internal Spack test).
if spack find --loaded --format "{variants}" heffte | grep -q "+rocm"; then
    HIP_DIR=$(spack find --format "{prefix}" hip | head -n 1)/lib/cmake/hip
    LLVM_DIR=$(spack find --format "{prefix}" llvm-amdgpu | head -n 1)/lib/cmake/AMDDeviceLibs
    COMGR_DIR=$(spack find --format "{prefix}" comgr | head -n 1)/lib/cmake/amd_comgr
    HSA_DEV=$(spack find --format "{prefix}" hsa-rocr-dev | head -n 1)
    ROCFFT_DIR=$(spack find --format "{prefix}" rocfft | head -n 1)/lib/cmake/rocfft

    CMAKE_OPTS+=("-Dhip_DIR=${HIP_DIR}")
    CMAKE_OPTS+=("-DAMDDeviceLibs_DIR=${LLVM_DIR}")
    CMAKE_OPTS+=("-Damd_comgr_DIR=${COMGR_DIR}")
    CMAKE_OPTS+=("-Dhsa-runtime64_DIR=${HSA_DEV}/lib/cmake/hsa-runtime64")
    CMAKE_OPTS+=("-DHSA_HEADER=${HSA_DEV}/include")
    CMAKE_OPTS+=("-Drocfft_DIR=${ROCFFT_DIR}")
fi

# Configure and compile
cmake ../testing "${CMAKE_OPTS[@]}"
make
