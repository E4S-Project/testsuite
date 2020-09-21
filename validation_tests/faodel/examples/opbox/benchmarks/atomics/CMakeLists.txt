set(PROJECT_NAME atomics)

set(HEADERS
   OpBenchmarkCompareSwap.hh
   OpBenchmarkFetchAdd.hh
)

set(SOURCES
   OpBenchmarkCompareSwap.cpp
   OpBenchmarkFetchAdd.cpp
   atomics.cpp
)


add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX )

target_link_libraries(${PROJECT_NAME} ${EXAMPLES_LIBS})

install(TARGETS ${PROJECT_NAME}
        EXPORT faodelExampleTargets
        RUNTIME DESTINATION "${BINARY_INSTALL_DIR}" COMPONENT bin
)
