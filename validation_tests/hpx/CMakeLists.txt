cmake_minimum_required(VERSION 3.17)
project(test CXX)
find_package(HPX 1.7.0 REQUIRED)

add_executable(test_future test_future.cpp)
target_link_libraries(test_future PRIVATE HPX::hpx)
target_include_directories(test_future PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(test_algorithm test_algorithm.cpp)
target_link_libraries(test_algorithm PRIVATE HPX::hpx)
target_include_directories(test_algorithm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(test_distributed test_distributed.cpp)
target_link_libraries(test_distributed PRIVATE HPX::hpx HPX::wrap_main
                                               HPX::iostreams_component)

if(HPX_WITH_CUDA)
  enable_language(CUDA)
  set_source_files_properties(test_gpu.cpp PROPERTIES LANGUAGE CUDA)
endif()
add_executable(test_gpu test_gpu.cpp)
target_link_libraries(test_gpu PRIVATE HPX::hpx)
target_include_directories(test_gpu PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
if(HPX_WITH_CUDA)
  target_compile_options(test_gpu PRIVATE --expt-extended-lambda --expt-relaxed-constexpr)
endif()
