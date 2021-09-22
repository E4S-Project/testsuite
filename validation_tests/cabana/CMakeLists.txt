cmake_minimum_required(VERSION 3.12)
project(CabanaExamples CXX)
find_package(Cabana REQUIRED)

add_executable(Cabana_Example example_aosoa.cpp)
target_link_libraries(Cabana_Example Cabana::cabanacore)
