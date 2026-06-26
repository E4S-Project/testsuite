#------------------------------------------------------------------------------#
# Copyright 2020 Kitware, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#------------------------------------------------------------------------------#

#cmake_minimum_required(VERSION 3.1)
cmake_minimum_required(VERSION 3.21)

cmake_policy(VERSION 3.21...${CMAKE_VERSION})
project(LegionExample_08_multiple_partitions)

 

# Only search if were building stand-alone and not as part of Legion
if(NOT Legion_SOURCE_DIR)
  find_package(Legion REQUIRED)
endif()

add_executable(legion legionTest.cc)
target_link_libraries(legion PRIVATE Legion::Legion)


if(Legion_USE_CUDA)
  find_package(CUDAToolkit REQUIRED)
  target_link_libraries(Legion::Legion INTERFACE CUDA::cudart)
  #target_link_libraries(legion PRIVATE CUDA::cudart)
elseif(Legion_USE_HIP)
  find_package(hip REQUIRED)
  #target_link_libraries(legion PRIVATE hip::host)
  target_link_libraries(Legion::Legion INTERFACE hip::host)
endif()


if(Legion_ENABLE_TESTING)
  add_test(NAME legion COMMAND ${Legion_TEST_LAUNCHER} $<TARGET_FILE:legion> ${Legion_TEST_ARGS})
endif()
