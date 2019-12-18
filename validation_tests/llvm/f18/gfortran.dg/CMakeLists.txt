#
# This is a cmake script for running (part of) the gfortran testsuite with any compiler.
# Copy into trunk/gcc/testsuite/gfortran.dg/ and run from there:
#
#     mkdir test_my_compiler; cd test_my_compiler
#     cmake .. -DFC=my_fortran_compiler
#     ctest -j$(nproc) -R CMP
#     ctest -j$(nproc) -R EXE
#
# Note that the script does not run the complete testsuite, but only those tests which comply with the following conditions:
# * runtime test ("dg-do run"); most compile-time tests check for gfortran-specific error messages
# * not expected to fail ("dg-shouldfail" / "xfail *-*-*")
# * not requiring extra compiler flags (those are compiler-depedent)
# * not requiring additional source files (often those are .c files, which may not be handled by the Fortran compiler)
#

project(gfortran_testsuite LANGUAGES NONE)

cmake_minimum_required(VERSION 3.4)

include(CTest)
enable_testing()           # needed for ctest
include(FindUnixCommands)  # bash!

if (NOT FC)
  message(FATAL_ERROR "please specify the Fortran compiler via -DFC=... !")
endif()

message(STATUS "using compiler: ${FC}")


### get a list of all files with runtime tests

execute_process(COMMAND grep -r "{.*dg-do\ run"
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE GREP_OUTPUT)

# convert to a list
string(REPLACE "\n" ";" GREP_LIST ${GREP_OUTPUT})

foreach(line IN LISTS GREP_LIST)
  if (line)
    # extract file name
    string(FIND ${line} ":" pos)
    string(SUBSTRING ${line} 0 ${pos}-1 file)
    list(APPEND FILE_LIST ${file})
  endif()
endforeach()

# make the list of file names unique and sorted
list(REMOVE_DUPLICATES FILE_LIST)
list(SORT FILE_LIST)

set(count 0)

### loop over all files, determine options, compile & run

foreach(file IN LISTS FILE_LIST)
  set(file_path "${CMAKE_SOURCE_DIR}/${file}")    # full path
  get_filename_component(file_name ${file} NAME_WE)  # file name without extension
  execute_process(COMMAND grep "{ dg-options" ${file}
                  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                  OUTPUT_VARIABLE options)
  execute_process(COMMAND grep "{ dg-additional-options" ${file}
                  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                  OUTPUT_VARIABLE addoptions)
  execute_process(COMMAND grep "{ dg-additional-sources" ${file}
                  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                  OUTPUT_VARIABLE addsources)
  execute_process(COMMAND grep "{ dg-shouldfail" ${file}
                  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                  OUTPUT_VARIABLE shouldfail)
  execute_process(COMMAND grep "run.*xfail \\*-\\*-\\*" ${file}
                  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                  OUTPUT_VARIABLE xfail)
  if (options)
    # parse compiler options
    string(REPLACE "\n" "" options ${options})
    string(FIND ${options} "dg-options" p1)
    string(FIND ${options} "}" p2)
    math(EXPR p1 "${p1}+11")
    math(EXPR p2 "${p2}-1")
    math(EXPR l "${p2}-${p1}")
    string(SUBSTRING ${options} ${p1} ${l} options)
    string(REPLACE "\"" "" options ${options})
  endif()
  if (addoptions)
    # parse additional compiler options
    string(REPLACE "\n" "" addoptions ${addoptions})
    string(FIND ${addoptions} "dg-additional-options" p1)
    string(FIND ${addoptions} "}" p2 REVERSE)
    math(EXPR p1 "${p1}+22")
    math(EXPR p2 "${p2}-1")
    math(EXPR l "${p2}-${p1}")
    string(SUBSTRING ${addoptions} ${p1} ${l} addoptions)
    string(REPLACE "\"" "" addoptions ${addoptions})
  endif()
  set(addsources_path "")
  if (addsources)
    # determine additional source files
    string(REPLACE "\n" "" addsources ${addsources})
    string(FIND ${addsources} "dg-additional-sources" p1)
    string(FIND ${addsources} "}" p2)
    math(EXPR p1 "${p1}+22")
    math(EXPR p2 "${p2}-1")
    math(EXPR l "${p2}-${p1}")
    string(SUBSTRING ${addsources} ${p1} ${l} addsources)
    string(REPLACE "\"" "" addsources ${addsources})
    string(REPLACE " " ";" addsources ${addsources})
    foreach(src IN LISTS addsources)
      string(APPEND addsources_path " ${CMAKE_SOURCE_DIR}/${src}")
    endforeach()
  endif()
  if (NOT (options OR addoptions OR addsources OR shouldfail OR xfail))
    if (file MATCHES "coarray/")
      # add coarray flags
      if (FC MATCHES "gfortran")
        string(APPEND options " -fcoarray=single")
      elseif (FC MATCHES "ifort")
         string(APPEND options " -coarray")
      endif()
    endif()
    if (FC MATCHES "ifort" AND (file MATCHES ".f03" OR file MATCHES ".f08"))
      # make ifort handle non-std extensions
      string(APPEND options " -Tf")
      string(APPEND addoptions " -free")
    endif()
    if (FC MATCHES "nagfor")
      # make nagfor use the same kind parameter values as gfortran
      string(APPEND options "-kind=byte")
      # add location of .inc include files to search path
      string(APPEND addoptions "-I ${CMAKE_SOURCE_DIR}")
    endif()
    math(EXPR count "${count}+1")
    message(STATUS "${count} : ${file} : ${addsources} : ${options} : ${addoptions}")
    # create the subdir in which the test is run
    set(wdir "${count}_${file_name}")
    execute_process(COMMAND mkdir -p ${wdir}
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    # compilation test
    add_test(NAME "CMP:${file}"
             COMMAND ${BASH} -c "${FC} ${options} ${file_path} ${addsources_path} ${addoptions}"
             WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/${wdir}")
    # execution test
    add_test(NAME "EXE:${file}"
             COMMAND ${BASH} -c "./a.out"
             WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/${wdir}")
    # set test properties (timeouts & dependencies)
    set_tests_properties("CMP:${file}" PROPERTIES TIMEOUT 10)
    set_tests_properties("EXE:${file}" PROPERTIES TIMEOUT 10)
    set_tests_properties("EXE:${file}" PROPERTIES DEPENDS "CMP:${file}")
    set_tests_properties("EXE:${file}" PROPERTIES REQUIRED_FILES "${CMAKE_BINARY_DIR}/${wdir}/a.out")
  endif()
endforeach()

message(STATUS "----------------------------------------")
message(STATUS "found ${count} test cases")
message(STATUS "using compiler: ${FC}")
message(STATUS "----------------------------------------")
