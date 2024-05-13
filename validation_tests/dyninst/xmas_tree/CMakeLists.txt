project(generic)

# include(ElfUtils)
find_package(Dyninst 13.0.0 REQUIRED)

enable_testing()
# "christmas tree" test Like the "christmas tree" packet attack, this turns on
# all the includes and library links
add_executable(g_xmas_tree xmas_tree.cpp)
target_link_libraries(g_xmas_tree PUBLIC Dyninst::dynC_API Dyninst::dyninstAPI
                                         Dyninst::stackwalk)
