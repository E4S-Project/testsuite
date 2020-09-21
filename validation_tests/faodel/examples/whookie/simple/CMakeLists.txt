

add_executable( whookie_bootstrap_example bootstrap_example.cpp )
target_link_libraries( whookie_bootstrap_example ${EXAMPLE_LIBS} )

add_executable( whookie_killit_example killit_example.cpp )
target_link_libraries( whookie_killit_example ${EXAMPLE_LIBS} )

install(TARGETS whookie_bootstrap_example whookie_killit_example
        EXPORT faodelExampleTargets
        RUNTIME DESTINATION "${BINARY_INSTALL_DIR}" COMPONENT bin
)