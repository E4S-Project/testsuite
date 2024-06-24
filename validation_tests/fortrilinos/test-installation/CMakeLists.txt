#----------------------------------*-CMake-*----------------------------------#
# Copyright 2020 UT-Battelle, LLC
# License-Filename: LICENSE
# SPDX-License-Identifier: BSD-3-Clause
#-----------------------------------------------------------------------------#

cmake_minimum_required(VERSION 3.12)
project(ForTrilinosInstallTest VERSION 0.0.1 LANGUAGES Fortran)

#-----------------------------------------------------------------------------#

find_package(ForTrilinos)

#-----------------------------------------------------------------------------#


function(fortrilinos_add_example target)
  add_executable("${target}" ${ARGN})
  target_link_libraries("${target}" ForTrilinos::ForTrilinos)
  if(ForTrilinos_USE_MPI)
    target_link_libraries("${target}" MPI::MPI_Fortran)
  endif()
endfunction()

fortrilinos_add_example(maps-and-vectors MapsAndVectors.F90)
fortrilinos_add_example(power-method PowerMethod.F90)
fortrilinos_add_example(downstream-app downstream-app.F90)


#-----------------------------------------------------------------------------#

