cmake_minimum_required(VERSION 3.0)
project(petsc-example DESCRIPTION "PETSc Example" LANGUAGES C)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../cmake)

set(PETSC_DIR "${PETSC_DIR}" CACHE PATH "Path to PETSc installation directory")

find_package(MPI REQUIRED)

set(CMAKE_C_COMPILER ${MPI_C_COMPILER})

find_package(PETSC REQUIRED)

add_executable(ex19 ex19.c)
target_link_libraries(ex19 PRIVATE PETSC::ALL)
target_include_directories(ex19 PRIVATE ${PETSC_DIR}/include)

find_library(MATH_LIBRARY NAMES m)
if(MATH_LIBRARY)
    target_link_libraries(ex19 PRIVATE ${MATH_LIBRARY})
endif()

install(TARGETS ex19 RUNTIME DESTINATION bin)
