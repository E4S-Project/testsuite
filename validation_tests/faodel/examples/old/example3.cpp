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

//Any node in the system can create a directory and assign nodes to it. This
//example uses a non-root node to create the directory, and then has all nodes
//pull the info. If mpi was run with more than 2 nodes, the child list will
//be printed from a node that isn't the root and isn't the generator.
void example3_remote_create(){

  string dir_path="/remote/created/thing";
  string dir_info="Volunteered Nodes";
  DirectoryInfo dir;
  bool ok;

  //Have host create the item
  if(G.mpi_rank==G.mpi_size-1){

    cout <<"\nExample 3: Remote Create\n";

    DirectoryInfo src_dir(dir_path, dir_info);

    //We can also plug in nodes annonymously and have it generate a name
    for(int i=200; i<203; i++)
      src_dir.Join( faodel::nodeid_t(i, internal_use_only)); //No label means autogen it

    ok = dirman::HostNewDir(src_dir);
    assert(ok && "Non-root couldn't host a new directory?");
  }

  //Wait until the item is created
  MPI_Barrier(MPI_COMM_WORLD);

  //Have each node grab the directory
  ok = dirman::GetDirectoryInfo(ResourceURL(dir_path), &dir);
  assert(ok && "Node could not retrieve directory");

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
