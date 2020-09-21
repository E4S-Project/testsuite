// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <iostream>

#include "faodel-common/Common.hh"
#include "opbox/OpBox.hh"

#include "Globals.hh"
#include "OpRinger.hh"

//Globals holds mpi info and manages connections (see ping example for info)
Globals G;

//The configuration used in this example (see ping example for info)
std::string default_config_string = R"EOF(
# Note: node_role is defined when we determine if this is a client or a server

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
  cout <<"My simple ring example\n";

  //Similar to the ping example, we need to register our op, create
  //a configuration, and start up all services.
  opbox::RegisterOp<OpRinger>();

  faodel::Configuration config(default_config_string);
  config.AppendFromReferences();

  G.StartAll(argc, argv, config);

  //The master node (rank 0) creates the request and issues it
  if(G.mpi_rank==0){

    //Our ring_info structure holds a list of nodes to visit as
    //well as the responses that have been submitted by each node.
    //We include everyone but ourself in the list. The Op is
    //configured to send the message back to the origin when the
    //last node in the list gets its copy
    RingInfo ring_info1;
    for(int i=1; i<G.mpi_size; i++)
      ring_info1.AddNewNode( G.nodes[i] );

    //Just like the ping example, we create a new op and launch it
    OpRinger *op1 = new OpRinger(ring_info1);
    future<RingInfo> fut1 = op1->GetFuture();
    opbox::LaunchOp(op1);

    //Block until the Op's future is set, then dump the result
    RingInfo ring_info2=fut1.get();
    cout<<"First result from ring is :\n"<<ring_info2.GetResults();

  }

  //Finally, do an mpi barrier to sync all nodes and then invoke shutdown procedures
  //to stop the FAODEL. Global also does an mpi finalize to close out the test.
  G.StopAll();

  return 0;
}
