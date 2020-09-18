set(PROJECT_NAME dim-sum)

set(HEADERS
  DimSum.hh
  WorkerThread.hh
  Producer.hh
  Consumer.hh
)

set(SOURCES
   DimSum.cpp
   WorkerThread.cpp
   Producer.cpp
   Consumer.cpp
   dim-sum.cpp
)


add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX )

target_link_libraries(${PROJECT_NAME} ${EXAMPLES_LIBS} )

install(TARGETS ${PROJECT_NAME}
        EXPORT faodelExampleTargets
        RUNTIME DESTINATION "${BINARY_INSTALL_DIR}" COMPONENT bin
)
