###############################################################################
# Copyright (c) 2016-24, Lawrence Livermore National Security, LLC
# and RAJA project contributors. See the RAJA/LICENSE file for details.
#
# SPDX-License-Identifier: (BSD-3-Clause)
###############################################################################

# Define the list of executables
set(EXAMPLES
  ex2_approx-pi
  ex2_approx-pi_solution
  ex5_line-of-sight
  ex5_line-of-sight_solution
  ex6_stencil-offset-layout
  ex6_stencil-offset-layout_solution
  ex8_tiled-matrix-transpose
  ex8_tiled-matrix-transpose_solution
  ex9_matrix-transpose-local-array
  ex9_matrix-transpose-local-array_solution
)

foreach(example IN LISTS EXAMPLES)
  
  # Standard CMake replacement for raja_add_executable
  add_executable(${example} ${example}.cpp)
  
  # Link against Spack's RAJA. This automatically applies compile definitions, 
  # include directories, and link dependencies (e.g., OpenMP, CUDA/HIP runtimes).
  target_link_libraries(${example} PRIVATE RAJA)

  # If using CUDA, we must tell CMake to compile these .cpp files using nvcc
  if (ENABLE_CUDA)
    set_source_files_properties(${example}.cpp PROPERTIES LANGUAGE CUDA)
  endif ()
  
endforeach()
