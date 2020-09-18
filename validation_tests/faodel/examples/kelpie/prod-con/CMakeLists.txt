set( HDF5_PREFER_PARALLEL TRUE )

add_executable( prod-con prod-con.cpp Globals.cpp )
target_link_libraries( prod-con ${EXAMPLES_LIBS} )

find_package( HDF5 COMPONENTS CXX HL )
if( HDF5_FOUND )
  target_link_libraries( prod-con ${HDF5_LIBRARIES} )
  target_include_directories( prod-con PUBLIC ${HDF5_INCLUDE_DIRS} )
  target_compile_definitions( prod-con PUBLIC ${HDF5_DEFINITIONS} -DHAVE_HDF5 )
endif()


install(TARGETS prod-con
        EXPORT faodelExampleTargets
        RUNTIME DESTINATION "${BINARY_INSTALL_DIR}" COMPONENT bin
        )


