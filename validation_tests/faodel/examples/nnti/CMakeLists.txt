##############################################################################
#          All Testing should be subordinate to src/tests directory          #
##############################################################################


# This function helps us do all the build/install options for the tests. Rather
# than have the user pipe in the source and target names, it asks for
# a modified version of the path/name of the test. eg
#
#  project/tests/component/tb_mything.cpp --> component/tb_mything
#
# The other arg specifies whether it should add it to the project's test list.
#
function( make_nnti_cpp_example   item  extra_cxx_flags extra_linker_flags )

    #Use the item name to build the target and source names      
    string(REGEX REPLACE ".*/" "CppExample" target ${item} )
    set(source ${CMAKE_CURRENT_LIST_DIR}/${item}.cpp)

    add_executable(        ${target} ${source} )
    target_link_libraries( ${target}
        cppexampleutils
        ${extra_linker_flags}
    )

    set_target_properties(${target} PROPERTIES COMPILE_FLAGS "${extra_cxx_flags}")
    set_target_properties(${target} PROPERTIES LINK_FLAGS    "${extra_linker_flags}")
    set_target_properties(${target} PROPERTIES OUTPUT_NAME   "${target}")

    install(TARGETS ${target}
            EXPORT faodelExampleTargets
            RUNTIME DESTINATION "${BINARY_INSTALL_DIR}" COMPONENT bin)
endfunction()



# Wire in all the source directories so testing can run as if
# headers were dumped in flat directory
#

    
set(cppexampleutils_target cppexampleutils)
add_library(${cppexampleutils_target} cpp/example_utils.hpp cpp/example_utils.cpp)

target_link_libraries(${cppexampleutils_target} Faodel::nnti )
set_target_properties(${cppexampleutils_target} PROPERTIES LINKER_LANGUAGE  CXX )


if (IBVerbs_FOUND)
make_nnti_cpp_example( cpp/PingPong  "" "-lz -lpthread"  )
endif()
