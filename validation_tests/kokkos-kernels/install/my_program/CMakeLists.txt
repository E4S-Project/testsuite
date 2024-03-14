
add_executable(myTest test.cc)
add_executable(KokkosSparse_wiki_spadd  KokkosSparse_wiki_spadd.cpp)
# I now link to the CMake-specific target
target_link_libraries(myTest PUBLIC Kokkos::kokkoskernels)
target_link_libraries(KokkosSparse_wiki_spadd PUBLIC Kokkos::kokkoskernels)

