###############################################################################
# Copyright (c) 2016-21, Lawrence Livermore National Security, LLC
# and RAJA project contributors. See the RAJA/LICENSE file for details.
#
# SPDX-License-Identifier: (BSD-3-Clause)
###############################################################################

set(example_depends RAJA)

if (ENABLE_OPENMP)
  set(example_depends
    ${example_depends}
    openmp)
endif ()

if (ENABLE_CUDA)
  set(example_depends
    ${example_depends}
    cuda)
endif ()

blt_add_executable(
  NAME example.exe
  SOURCES example.cpp
  DEPENDS_ON ${example_depends})


blt_add_executable(
  NAME ex1_vector-addition_solution
  SOURCES ex1_vector-addition_solution.cpp
  DEPENDS_ON ${example_depends})

blt_add_executable(
  NAME ex2_approx-pi_solution
  SOURCES ex2_approx-pi_solution.cpp
  DEPENDS_ON ${example_depends})

blt_add_executable(
  NAME ex3_colored-indexset_solution
  SOURCES ex3_colored-indexset_solution.cpp
  DEPENDS_ON ${example_depends} )

blt_add_executable(
  NAME ex4_atomic-histogram_solution
  SOURCES ex4_atomic-histogram_solution.cpp
  DEPENDS_ON ${example_depends}  )

blt_add_executable(
  NAME ex5_line-of-sight_solution
  SOURCES ex5_line-of-sight_solution.cpp
  DEPENDS_ON ${example_depends}  )

blt_add_executable(
  NAME ex6_stencil-offset-layout_solution
  SOURCES ex6_stencil-offset-layout_solution.cpp
  DEPENDS_ON ${example_depends})

blt_add_executable(
  NAME ex7_nested-loop-reorder_solution
  SOURCES ex7_nested-loop-reorder_solution.cpp
  DEPENDS_ON ${example_depends}  )

blt_add_executable(
  NAME ex8_tiled-matrix-transpose_solution
  SOURCES ex8_tiled-matrix-transpose_solution.cpp
  DEPENDS_ON ${example_depends} )

blt_add_executable(
  NAME ex9_matrix-transpose-local-array_solution
  SOURCES ex9_matrix-transpose-local-array_solution.cpp
  DEPENDS_ON ${example_depends})
