// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <iostream>

#include "faodel-common/Common.hh"
//#include "faodel-common/TimeLogger.hh"

#include "opbox/OpBox.hh"

#include "Globals.hh"
#include "OpRapidFire.hh"

//Globals holds mpi info and manages connections (see ping example for info)
Globals G;

//The configuration used in this example (see ping example for info)
std::string default_config_string = R"EOF(
# Note: node_role is defined when we determine if this is a client or a server

master.whookie.port   7777
server.whookie.port   1991

dirman.type           centralized
dirman.root_role      master

#target.dirman.host_root
#target.dirman.write_to_file ./dirman.txt

#bootstrap.debug true
#whookie.debug   true
#opbox.debug     true
#dirman.debug    true

)EOF";

using namespace std;


int main(int argc, char **argv){

  //This is a simple example of how to do a ring of communication
  //cout <<"Simple rapidfire example (use one op to send many ping pong messages\n";

  //Similar to the ping example, we need to register our op, create
  //a configuration, and start up all services.
  opbox::RegisterOp<OpRapidFire>();

  faodel::Configuration config(default_config_string);
  config.AppendFromReferences();

  G.StartAll(argc, argv, config);

  //The master node (rank 0) creates the request and issues it
  if(G.mpi_rank==0){


    for(int i=0; i<10; i++){
//      faodel::TimeLogger timer;

      OpRapidFire *op = new OpRapidFire(G.nodes[1], 32, true);
      future<int> fut1 = op->GetFuture();
      opbox::LaunchOp(op);

//      timer.Mark(0);

      int num_sent=fut1.get();
//      timer.Mark(1);
//      timer.Dump();
    }

  }

  //Finally, do an mpi barrier to sync all nodes and then invoke shutdown procedures
  //to stop the FAODEL. Global also does an mpi finalize to close out the test.
  G.StopAll();

  return 0;
}
