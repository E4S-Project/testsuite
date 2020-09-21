
if( NOT Faodel_BUILT_WITH_MPI )
  message( STATUS "The installed Faodel was not built with MPI, so Lunasa examples cannot be built" )
else()
  set( EXAMPLES_LIBS Faodel::lunasa )

  add_subdirectory( bootstrap )
  if(Faodel_NETWORK_LIBRARY STREQUAL "nnti")
    add_subdirectory( simple_ldo_send )
    add_subdirectory( simple_user_ldo_put )
  endif()
endif()
