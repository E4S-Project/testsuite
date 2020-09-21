
include_directories(
   support
)

#-- Support Lib----------------------------------------------------------------
add_library(mpi_support
            support/Globals.hh
            support/Globals.cpp
            support/SimpleDataStore.hh
            support/SimpleDataStore.cpp
)
set_target_properties( mpi_support PROPERTIES LINKER_LANGUAGE CXX )
target_link_libraries( mpi_support
                       MPI::MPI_CXX
                       Faodel::common )

set(EXAMPLES_LIBS
  mpi_support
  Faodel::opbox
  Boost::program_options
)

add_subdirectory( basic/my_simple_ping )
add_subdirectory( basic/rdma_ping )
add_subdirectory( basic/message_packing )

add_subdirectory( collectives/rapidfire )
add_subdirectory( collectives/ringer )
add_subdirectory( collectives/scatter_gather )

add_subdirectory( advanced/dirty_use )
add_subdirectory( advanced/job2job )

add_subdirectory( benchmarks )
