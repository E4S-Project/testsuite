// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

// The mpisyncstart service is a simple service for starting up jobs that
// use MPI. If you enable the service it will (in the least) do a barrier
// operation at startup.
//
// Note: mpisyncstart is much more useful if you want to share information
//       between nodes (eg, the dirman root). See the advanced example for
//       more info.


#include <iostream>
#include <mpi.h>

#include "faodel-common/Common.hh"
#include "faodel-common/Bootstrap.hh"
#include "faodel-services/MPISyncStart.hh"

using namespace std;
using namespace faodel;


int main(int argc, char **argv) {

  //Note: you MUST initialize mpi before starting this service
  int mpi_rank, mpi_size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

  faodel::Configuration config;
  config.Append("bootstrap.debug",     "true");
  config.Append("mpisyncstart.debug",  "true");
  config.Append("mpisyncstart.enable", "true"); //Force mpisyncstart to run at start.

  //Running with the mpisyncstart service enabled will cause an mpi barrier operation
  faodel::bootstrap::Start(config, faodel::mpisyncstart::bootstrap);
  faodel::bootstrap::Finish();

  MPI_Finalize();

}
