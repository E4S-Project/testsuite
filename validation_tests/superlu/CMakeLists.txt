cmake_minimum_required(VERSION 3.10)

# set the project name
project(superlu5x5)

# find the required package
find_package(superlu REQUIRED)

# specify the C standard
set(CMAKE_C_STANDARD 99)

find_library(METIS_LIBRARY NAMES metis REQUIRED)

link_directories(${superlu_DIR}/../../../lib)


# add the executable
add_executable(superlu5x5 superlu.c)

# include the superlu headers
target_include_directories(superlu5x5 PRIVATE ${superlu_DIR}/../../../include)
# link the superlu library
target_link_libraries(superlu5x5 PRIVATE superlu ${METIS_LIBRARY} ${BLAS_LIBRARIES} m dl)
