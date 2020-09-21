// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <iostream>
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

//The directory manager is also setup so nodes can ask to join an existing
//directory. In this test a node creates an empty directory that all the
//other nodes will join.
//
// Note: Since nodes join the list dynamically, the results you get back
//       when you join may not include everyone. Example 5 shows how you
//       can poll the directory until it has everything you need.
void example4_dynamic_joining(){

  string dir_path="/dynamic/joining/example";
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
  ok = dirman::JoinDirWithoutName(ResourceURL(dir_path));
  assert(ok && "Could not join the directory?");

  //Wait until everyone has joined.
  MPI_Barrier(MPI_COMM_WORLD);

  //Now we can fetch the directory and see everyone
  ok = dirman::GetRemoteDirectoryInfo(ResourceURL(dir_path), &dir);
  assert(ok && "");

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
