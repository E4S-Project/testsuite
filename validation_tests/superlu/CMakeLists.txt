cmake_minimum_required(VERSION 3.10)

# set the project name
project(c_sample)

# find the required package
find_package(superlu REQUIRED)

# specify the C standard
set(CMAKE_C_STANDARD 99)

link_directories(${superlu_DIR}/../../../lib)


# add the executable
add_executable(c_sample c_sample.c)

# include the superlu headers
target_include_directories(c_sample PRIVATE ${superlu_DIR}/../../../include)
# link the superlu library
target_link_libraries(c_sample PRIVATE superlu ${BLAS_LIBRARIES} m dl)
