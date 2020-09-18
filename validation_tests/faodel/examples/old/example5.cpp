// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <iostream>
#include <chrono>
#include <thread>

#include <unistd.h>
#include <assert.h>

#include "faodel-common/Common.hh"
#include "opbox/OpBox.hh"

#include "opbox/services/dirman/DirectoryManager.hh"

#include "Globals.hh"

using namespace std;
using namespace faodel;
using namespace opbox;

extern Globals G;


void example5_polling(){

  string dir_path="/dynamic/joining/with/polling";
  string dir_info="Self Joining Nodes";
  DirectoryInfo dir;
  bool ok;

  //Have host create the item
  if(G.mpi_rank==G.mpi_size-1){

    cout <<"\nExample 4: Dynamic Joining\n";

    //Create an empty spot for things to live
    ok = dirman::HostNewDir(DirectoryInfo(dir_path, dir_info));
    assert(ok && "Non-root couldn't host a new directory?");
  }

  //Wait until the item is created
  MPI_Barrier(MPI_COMM_WORLD);

  //Have all nodes join the resource. Ignore the returned info, as
  //it will not be complete.
  ok = dirman::JoinDirWithoutName(ResourceURL(dir_path), &dir);
  assert(ok && "Could not join the directory?");

  //No mpi barrier here. Immediately start checking for capacity

  //Poll until we have the right number of members
  while(dir.members.size() < G.mpi_size){
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ok = dirman::GetRemoteDirectoryInfo(ResourceURL(dir_path), &dir);
    assert(ok && "");
    cout <<"Rank "<<G.mpi_rank<<" sees "<<dir.members.size()<<" members\n";
  }


  //Dump info
  cout <<"Info: '"<<dir.info
       <<"' ReferenceNode: " <<dir.GetReferenceNode().GetHex()
       <<" NumberMembers: " <<dir.members.size()<<endl;

  MPI_Barrier(MPI_COMM_WORLD);

  //Want to do test on a node that isn't the root or the node that generated the data
  int test_id = G.mpi_size-2;
  if(test_id<0) test_id=0;

  if(G.mpi_rank==test_id){
    cout <<"Rank "<<G.mpi_rank<<" sees the following members:\n";
    for(auto &name_node : dir.members){
      cout <<"     "<<name_node.name
           <<"  "<<name_node.node.GetHex() <<endl;
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);

}
