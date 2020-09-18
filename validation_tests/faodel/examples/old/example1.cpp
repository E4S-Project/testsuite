// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <iostream>
#include <unistd.h>
#include <assert.h>
#include <mpi.h>

#include "faodel-common/Common.hh"
#include "opbox/OpBox.hh"

#include "dirman/DirMan.hh"

#include "Globals.hh"

using namespace std;
using namespace faodel;
using namespace opbox;



//Have the root node create a directory, and have some of the nodes join it
void example1_create_and_fetch(){

  string dir_path="/this/dir/created/by/root";
  string dir_info="Stuff about resource clients would want to know";
  DirectoryInfo dir;
  bool ok;

  //Have host create the item
  if(G.mpi_rank==0){

    cout <<"\nExample 1: Create and Fetch\n";

    ok = dirman::HostNewDir(DirectoryInfo(dir_path, dir_info));
    assert(ok && "Root couldn't host a new directory?");
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
}
