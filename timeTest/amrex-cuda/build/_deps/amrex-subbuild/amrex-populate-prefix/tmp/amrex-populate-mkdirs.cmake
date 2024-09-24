# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/ubuntu/testsuite/validation_tests/amrex-cuda/build/_deps/amrex-src"
  "/home/ubuntu/testsuite/validation_tests/amrex-cuda/build/_deps/amrex-build"
  "/home/ubuntu/testsuite/validation_tests/amrex-cuda/build/_deps/amrex-subbuild/amrex-populate-prefix"
  "/home/ubuntu/testsuite/validation_tests/amrex-cuda/build/_deps/amrex-subbuild/amrex-populate-prefix/tmp"
  "/home/ubuntu/testsuite/validation_tests/amrex-cuda/build/_deps/amrex-subbuild/amrex-populate-prefix/src/amrex-populate-stamp"
  "/home/ubuntu/testsuite/validation_tests/amrex-cuda/build/_deps/amrex-subbuild/amrex-populate-prefix/src"
  "/home/ubuntu/testsuite/validation_tests/amrex-cuda/build/_deps/amrex-subbuild/amrex-populate-prefix/src/amrex-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/ubuntu/testsuite/validation_tests/amrex-cuda/build/_deps/amrex-subbuild/amrex-populate-prefix/src/amrex-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/ubuntu/testsuite/validation_tests/amrex-cuda/build/_deps/amrex-subbuild/amrex-populate-prefix/src/amrex-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
