cmake_minimum_required(VERSION 3.18)

if(POLICY CMP0144)
  cmake_policy(SET CMP0144 NEW)
endif()

project(AMReX_Validation_Test C CXX)

if (AMReX_GPU_BACKEND STREQUAL "HIP")
    find_package(hip REQUIRED)
    find_package(hiprand REQUIRED)
    find_package(rocrand REQUIRED)
    find_package(rocprim REQUIRED)
    find_package(rocsparse REQUIRED)
    find_package(rocfft REQUIRED)
endif()

find_package(AMReX REQUIRED)

if (AMReX_GPU_BACKEND STREQUAL "CUDA")
    enable_language(CUDA)
endif()

add_executable(amrex_test main.cpp)
target_link_libraries(amrex_test PRIVATE AMReX::amrex)

if (AMReX_GPU_BACKEND STREQUAL "CUDA")
    # AMReXTargetHelpers isn't auto-included by AMReXConfig.cmake in this AMReX
    # version, but AMReXConfig.cmake does add it to CMAKE_MODULE_PATH for us.
    include(AMReXTargetHelpers)

    # Sets CUDA_SEPARABLE_COMPILATION (correctly, matching AMReX's AMReX_GPU_RDC),
    # converts main.cpp to CUDA language, and sets CUDA_ARCHITECTURES from AMReX's
    # own build (AMREX_CUDA_ARCHS) rather than us re-deriving it from SPACK_CUDA_ARCH.
    setup_target_for_cuda_compilation(amrex_test)

    # Belt-and-suspenders: ensures the device-link step actually pulls in and
    # resolves device symbols from linked static libs (libamrex_*.a). Executable
    # targets default to ON for this per CMake docs, but AMReX's static libs are
    # IMPORTED targets without CUDA_SEPARABLE_COMPILATION set on them, so make it explicit.
    set_target_properties(amrex_test PROPERTIES CUDA_RESOLVE_DEVICE_SYMBOLS ON)
endif ()
