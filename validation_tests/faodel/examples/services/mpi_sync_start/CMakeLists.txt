set(PROJECT_NAME mpi_sync_start_examples)




add_executable(mpi_sync_start_basic mpi_sync_start_basic.cpp)
set_target_properties(mpi_sync_start_basic PROPERTIES LINKER_LANGUAGE CXX )
target_link_libraries(mpi_sync_start_basic ${EXAMPLES_LIBS})

add_executable(mpi_sync_start_advanced mpi_sync_start_advanced.cpp)
set_target_properties(mpi_sync_start_advanced PROPERTIES LINKER_LANGUAGE CXX )
target_link_libraries(mpi_sync_start_advanced ${EXAMPLES_LIBS})

install(TARGETS mpi_sync_start_basic mpi_sync_start_advanced
        EXPORT faodelExampleTargets
        RUNTIME DESTINATION "${BINARY_INSTALL_DIR}" COMPONENT bin
        )
