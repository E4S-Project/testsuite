set(PROJECT_NAME message_packing)

set(HEADERS
)

set(SOURCES
   message_packing_example.cpp
   example1.cpp
   example2.cpp
   example3.cpp
   example4.cpp
)

set( FLATBUFFERS_ROOT $ENV{FLATBUFFERS_ROOT} )
if( FLATBUFFERS_ROOT )
  # try to include the flatbuffers example
  find_library( FLATBUFFERS_LIB flatbuffers HINTS $ENV{FLATBUFFERS_ROOT}/lib )
  if( FLATBUFFERS_LIB )
    message( STATUS "Will build FlatBuffers message-packing example using FlatBuffers from $ENV{FLATBUFFERS_ROOT}" )
    include_directories( $ENV{FLATBUFFERS_ROOT}/include )
    set( EXTRA_EXAMPLE_LIBS ${EXTRA_EXAMPLE_LIBS} ${FLATBUFFERS_LIB} )
    set( HEADERS ${HEADERS} fancypants_generated.h )
    set( SOURCES ${SOURCES} example5.cpp )
    add_definitions( -DUSE_FLATBUFFERS )

    add_custom_command(
      OUTPUT fancypants_generated.h
      MAIN_DEPENDENCY fancypants.fbs
      COMMAND $ENV{FLATBUFFERS_ROOT}/bin/flatc --cpp ${CMAKE_CURRENT_SOURCE_DIR}/fancypants.fbs
      VERBATIM )
  endif()
endif()

set( CAPNPROTO_ROOT $ENV{CAPNPROTO_ROOT} )
if( CAPNPROTO_ROOT )
  # try to include the Cap'n Proto example
  find_library( CAPNPROTO_LIB capnp HINTS $ENV{CAPNPROTO_ROOT}/lib )
  if( CAPNPROTO_LIB )
    message( STATUS "Will build Cap'n Proto message-packing example using Cap'n Proto from $ENV{CAPNPROTO_ROOT}" )
    include_directories( $ENV{CAPNPROTO_ROOT}/include )
    find_library( KJ_LIB kj HINTS $ENV{CAPNPROTO_ROOT}/lib )
    set( EXTRA_EXAMPLE_LIBS ${EXTRA_EXAMPLE_LIBS} ${CAPNPROTO_LIB} ${KJ_LIB} )
    set( HEADERS ${HEADERS} fancypants.capnp.h )
    set( SOURCES ${SOURCES} example6.cpp fancypants.capnp.c++ )
    add_definitions( -DUSE_CAPNPROTO )

    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/fancypants.capnp.h ${CMAKE_CURRENT_SOURCE_DIR}/fancypants.capnp.c++
      MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/fancypants.capnp
      COMMAND $ENV{CAPNPROTO_ROOT}/bin/capnp compile --output=$ENV{CAPNPROTO_ROOT}/bin/capnpc-c++ ${CMAKE_CURRENT_SOURCE_DIR}/fancypants.capnp
      VERBATIM )
  endif()
endif()

set( CEREAL_ROOT $ENV{CEREAL_ROOT} )
if( CEREAL_ROOT )
  # try to include the Cereal example
  find_path( CEREAL binary.hpp HINTS $ENV{CEREAL_ROOT}/include/cereal/archives )
  if( CEREAL )
    message( STATUS "Will build Cereal message-packing example using Cereal from $ENV{CEREAL_ROOT}" )
    include_directories( $ENV{CEREAL_ROOT}/include )
    set( SOURCES ${SOURCES} example7.cpp )
    add_definitions( -DUSE_CEREAL )
  endif()
endif()

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX )

target_link_libraries(${PROJECT_NAME} ${EXAMPLES_LIBS} ${EXTRA_EXAMPLE_LIBS})

install(TARGETS ${PROJECT_NAME}
        EXPORT faodelExampleTargets
        RUNTIME DESTINATION "${BINARY_INSTALL_DIR}" COMPONENT bin
)
