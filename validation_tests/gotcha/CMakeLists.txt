cmake_minimum_required(VERSION 3.8)

project(using_gotcha_with_cmake)

if(NOT DEFINED GOTCHA_DIR OR (NOT EXISTS ${GOTCHA_DIR}/lib/cmake/gotcha AND NOT EXISTS  ${GOTCHA_DIR}/lib64/cmake/gotcha))
	message(FATAL_ERROR "Missing required 'GOTCHA_DIR' variable pointing to an installed gotcha")
endif()

find_package(gotcha REQUIRED
             NO_DEFAULT_PATH
	     PATHS ${GOTCHA_DIR}/lib/cmake ${GOTCHA_DIR}/lib64/cmake)


add_subdirectory(autotee)
add_subdirectory(minimal)
