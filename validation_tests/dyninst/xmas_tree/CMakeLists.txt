cmake_minimum_required(VERSION 3.4)
project(generic)

find_package(Dyninst REQUIRED)
find_package(Boost REQUIRED)

set (CMAKE_MODULE_PATH "${Dyninst_DIR}" "${Dyninst_DIR}/Modules" ${CMAKE_MODULE_PATH})
include(ThreadingBuildingBlocks)


# "christmas tree" test Like the "christmas tree" packet attack, this turns on
# all the includes and library links
add_executable(xmas_tree xmas_tree.cpp)

target_include_directories(
  xmas_tree PRIVATE ${DYNINST_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${TBB_INCLUDE_DIRS})

target_link_libraries(
  xmas_tree
  PUBLIC common
         dynC_API
         dynDwarf
         dynElf
         dyninstAPI
         instructionAPI
         parseAPI
         patchAPI
         pcontrol
         stackwalk
         symLite
         symtabAPI)
