# ---------------------------------------------------------------
# Programmer: Cody J. Balos, David J. Gardner  @ LLNL
# ---------------------------------------------------------------
# SUNDIALS Copyright Start
# Copyright (c) 2002-2019, Lawrence Livermore National Security
# and Southern Methodist University.
# All rights reserved.
#
# See the top-level LICENSE and NOTICE files for details.
#
# SPDX-License-Identifier: BSD-3-Clause
# SUNDIALS Copyright End
# ---------------------------------------------------------------

# check if the SUNDIALS path is set
if(NOT SUNDIALS_DIR)
  message(FATAL_ERROR "Error: SUNDIALS_DIR not set!")
  set(SUNDIALS_DIR "" CACHE PATH "SUNDIALS install directory")
endif()

# determine SUNDIALS components needed
if(NOT SUNDIALS_FIND_COMPONENTS)
  set(SUNDIALS_FIND_COMPONENTS
    "arkode"
    "cvode"
    "nvecserial"
    "nvecparallel")
endif()

if(ENABLE_SUPERLU)
  list(APPEND SUNDIALS_FIND_COMPONENTS "sunmatrixslunrloc")
  list(APPEND SUNDIALS_FIND_COMPONENTS "sunlinsolsuperludist")
endif()
if(ENABLE_PETSC)
  list(APPEND SUNDIALS_FIND_COMPONENTS "nvecpetsc")
  list(APPEND SUNDIALS_FIND_COMPONENTS "sunnonlinsolpetscsnes")
endif()

# find the library for each component
foreach(component ${SUNDIALS_FIND_COMPONENTS})
  find_library(${component}_LIBRARY sundials_${component}
    PATHS ${SUNDIALS_DIR}/lib ${SUNDIALS_DIR}/lib64
    NO_DEFAULT_PATH)
  if(${component}_LIBRARY)
    message(STATUS "Looking for SUNDIALS ${component}... success")
    list(APPEND SUNDIALS_LIBRARIES ${${component}_LIBRARY})
    list(APPEND SUNDIALS_REQUIRED_VARS ${component}_LIBRARY)
    set(SUNDIALS_${component}_FOUND TRUE)
  else()
    message(STATUS "Looking for SUNDIALS ${component}... failed")
  endif()
endforeach()

find_package_handle_standard_args(SUNDIALS
  REQUIRED_VARS ${SUNDIALS_REQUIRED_VARS}
  HANDLE_COMPONENTS)

# create a target for each component
if(SUNDIALS_FOUND)
  foreach(component ${SUNDIALS_FIND_COMPONENTS})
    if(NOT TARGET SUNDIALS::${component})
      add_library(SUNDIALS::${component} UNKNOWN IMPORTED)
      set_target_properties(SUNDIALS::${component}
        PROPERTIES
        IMPORTED_LOCATION ${${component}_LIBRARY}
        INTERFACE_INCLUDE_DIRECTORIES ${SUNDIALS_DIR}/include)
    endif()
  endforeach()
endif()
