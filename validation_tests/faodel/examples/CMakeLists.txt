cmake_minimum_required(VERSION 3.8.0)

project(faodel_examples LANGUAGES CXX C)

# Policy assertions
if( POLICY CMP0074 )
    cmake_policy( SET CMP0074 NEW ) # find_package() uses PackageName_ROOT vars
endif()

# By default, cmake looks in your ~/.cmake directory for information
# about packages. While that should be fine for most users, we disable
# it in these examples because developers often have different versions
# of libraries in different places and cmake always guesses wrong.
set( CMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY TRUE )
set( BINARY_INSTALL_DIR  bin  CACHE PATH "Installation directory for executables"  )


find_package( Faodel CONFIG REQUIRED )


add_subdirectory( common )
add_subdirectory( whookie )

if( Faodel_ENABLE_MPI_SUPPORT )
  # These examples have some MPI requirements, so don't build if no mpi

  add_subdirectory( services )
  if( Faodel_BUILT_Faodel_NETWORK_LIBRARY STREQUAL "nnti" )
    add_subdirectory( nnti )
  endif()
  add_subdirectory( lunasa )
  add_subdirectory( opbox )
  add_subdirectory( dirman )
  add_subdirectory( kelpie )
endif()

