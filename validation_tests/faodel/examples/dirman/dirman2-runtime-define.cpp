// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

// This test shows how to
//  1. Use dirman to get a list of all the nodes in the system
//  2. Have a node volunteer other nodes to be in a resource
//
// TODO: This was observed to have some crashes at one point. Unclear if resolved.

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

mpisyncstart.debug         true

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


  //For this job, all we want to do is use rank 0 as the dir root, and
  //create a list of all the ranks (in order) in an entry called /nodelist/all.
  faodel::Configuration config(default_config_string);
  config.Append("dirman.root_node_mpi", "0");
  config.Append("dirman.resources_mpi[]", "allstuff:/nodelist/all    all");

  mpisyncstart::bootstrap(); //Enable the mpisyncstart service, which will handle the above

  faodel::bootstrap::Start(config, dirman::bootstrap); //Start our service

  //We're started now. We can the /nodelist/all entry we created at start
  //time to provide a complete list of ranks->nodes. Nodes are named
  //"AGx" in this list, where x is the rank, and AG means "Auto Generated name".
  DirectoryInfo all_nodes;
  dirman::GetDirectoryInfo(ResourceURL("ref:/nodelist/all"), &all_nodes);



  //In addition to defining resources at init time, a node can create a new
  //resource and volunteer other nodes to be a part of it. In this example
  //we have the last rank create resource out of the even nodes.
  if(mpi_rank == mpi_size-1) {

    cout <<"Got list of nodes. It has "<<all_nodes.members.size()<<" ranks\n";
    DirectoryInfo new_dir("ref:/nodelist/odd","My odd nodes");
    for(int i=1; i<all_nodes.members.size(); i+=2) {
      string name = "Rank"+to_string(i);
      new_dir.Join(all_nodes.members[i].node, name);
    }
    dirman::HostNewDir(new_dir);
  }

  MPI_Barrier(MPI_COMM_WORLD);

  DirectoryInfo odd_dir;
  dirman::GetDirectoryInfo(ResourceURL("ref:/nodelist/odd"), &odd_dir);


  for(int i=0; i<mpi_size; i++) {
      if(mpi_rank==i){
        cout <<"Rank "<<i<<" sees: "<<odd_dir.str(4,4);
      }
      MPI_Barrier(MPI_COMM_WORLD);
  }

  MPI_Barrier(MPI_COMM_WORLD);


  faodel::bootstrap::Finish();

  MPI_Finalize();

  return 0;
}
