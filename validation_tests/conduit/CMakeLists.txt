# Copyright (c) Lawrence Livermore National Security, LLC and other Conduit
# Project developers. See top-level LICENSE AND COPYRIGHT files for dates and
# other details. No copyright assignment is required to contribute to Conduit.
###############################################################################

cmake_minimum_required(VERSION 3.14)

project(using_with_cmake)


################################################################
# import conduit using find_package search

find_package(Conduit REQUIRED)


######
# If Conduit was built with c++11 support, make sure we enable it
######
if(CONDUIT_USE_CXX11)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

#######################################################
# create our examples
#######################################################

add_executable(conduit_example conduit_example.cpp)

# link to conduit mpi targets 
target_link_libraries(conduit_example conduit::conduit)


add_executable(conduit_mpi_example conduit_mpi_example.cpp)

# link to conduit mpi targets 
target_link_libraries(conduit_mpi_example conduit::conduit
                                          conduit::conduit_mpi
                                          MPI::MPI_CXX)


