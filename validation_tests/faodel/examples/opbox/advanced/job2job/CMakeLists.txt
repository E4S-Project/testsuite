set(PROJECT_NAME job2job)

set(HEADERS
   OpInterjobPing.hh
)


set(COMMON_SOURCES
   OpInterjobPing.cpp
)
set(SERVER_SOURCES
   OpInterjobPing.cpp
   job2job-server.cpp
)
set(CLIENT_SOURCES
   OpInterjobPing.cpp
   job2job-client.cpp
)

set(SERVER_EXEC ${PROJECT_NAME}-server)
set(CLIENT_EXEC ${PROJECT_NAME}-client)

add_executable(${SERVER_EXEC} ${HEADERS} ${SERVER_SOURCES} ${COMMON_SOURCES})
set_target_properties(${SERVER_EXEC} PROPERTIES LINKER_LANGUAGE CXX )
target_link_libraries(${SERVER_EXEC} ${EXAMPLES_LIBS})

add_executable(${CLIENT_EXEC} ${HEADERS} ${CLIENT_SOURCES} ${COMMON_SOURCES})
set_target_properties(${CLIENT_EXEC} PROPERTIES LINKER_LANGUAGE CXX )
target_link_libraries(${CLIENT_EXEC} ${EXAMPLES_LIBS})

install(TARGETS ${PROJECT_NAME}-server ${PROJECT_NAME}-client
        EXPORT faodelExampleTargets
        RUNTIME DESTINATION "${BINARY_INSTALL_DIR}" COMPONENT bin
)