// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <iostream>
#include <unistd.h>
#include <mpi.h>
#include <assert.h>

#include "faodel-common/Common.hh"
#include "faodel-services/MPISyncStart.hh"
#include "dirman/DirMan.hh"


std::string default_config_string = R"EOF(

# Select the type of dirman to use. Currently we only have centralized, which
# just sticks all the directory info on one node (called root).
dirman.type           centralized

# Turn these on if you want to see more debug messages
#bootstrap.debug           true
#whookie.debug             true
#opbox.debug               true
#dirman.debug              true
#dirman.cache.others.debug true
#dirman.cache.mine.debug   true

#mpisyncstart.debug         true

)EOF";

using namespace std;
using namespace faodel;


int main(int argc, char **argv){

  //Initialize MPI before doing anything
  int mpi_rank, mpi_size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

  if(mpi_size<4){
    if(mpi_rank==0)
      cout <<"This should be run with four or more ranks\n";
    MPI_Finalize();
    exit(0);
  }


  //If you're running an mpi job the easiest way to define resources is to
  //plug the info into the configuration and use mpisyncstart to convert
  //ranks to nodeid's. The mpisyncstart service looks at the configuration
  //for certain keyworkds that have an "_mpi" suffix. If it detects one or
  //more of these tokens, it does some mpi work to learn what everyone's
  //nodeids are, and then modify the config with the updated info.
  faodel::Configuration config(default_config_string);

  //Specify the root node is going to live at mpi rank 0
  config.Append("dirman.root_node_mpi", "0");

  //Specify 3 different resources to preload into the dirman root. We use
  //the [] suffix to tell config this keyword is a vector of items. The
  //resource url is the base url that will be used for dirinfo. The end of
  //the string is the list of ranks to use. you can specify simple ranges,
  //comma-separated lists, and/or simple keywords like all, middle, and end.
  //             resource keyword       |  resource url        | which ranks belong
  //             -----------------------|----------------------|--------------------
  config.Append("dirman.resources_mpi[]", "thing1:/my/thing1     all");
  config.Append("dirman.resources_mpi[]", "thing2:/my/thing2     0-middle");
  config.Append("dirman.resources_mpi[]", "thing3:/other/thing3  end");
  config.Append("dirman.resources_mpi[]", "thing4:/other/thing4  1,2");


  mpisyncstart::bootstrap(); //Enable the mpisyncstart service, which will handle the above

  faodel::bootstrap::Start(config, dirman::bootstrap); //Start our service

  //Now that things are started, dir_root should now be preloaded with
  //info. The other nodes do NOT know this info though, so they need to
  //do lookup to learn where things are.

  //Note: when you look things up, you can specify "ref:" for the type
  //      to designate that this is a reference.
  vector<string> refs = { "ref:/my/thing1",    "ref:/my/thing2",
                          "ref:/other/thing3", "ref:/other/thing4"};

  for(auto rname : refs) {
    if(mpi_rank==0)
      cout <<"Reference "<<rname<<"==============================================\n";
    for (int i = 0; i < mpi_size; i++) {
      if (i == mpi_rank) {
        cout << "Rank " << i << " Observations:\n";

        DirectoryInfo dir;
        rc_t rc = dirman::GetDirectoryInfo(ResourceURL(rname), &dir);
        cout << dir.str(4, 4);
      }
      MPI_Barrier(MPI_COMM_WORLD);
    }
  }


  faodel::bootstrap::Finish();

  MPI_Finalize();

  return 0;
}
