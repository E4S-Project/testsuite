set(PROJECT_NAME my_simple_ping)

set(HEADERS
   OpMySimplePing.hh
)

set(SOURCES
   OpMySimplePing.cpp
   my_simple_ping.cpp
)


add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX )

target_link_libraries(${PROJECT_NAME} ${EXAMPLES_LIBS})

install(TARGETS ${PROJECT_NAME}
        EXPORT faodelExampleTargets
        RUNTIME DESTINATION "${BINARY_INSTALL_DIR}" COMPONENT bin
)
