// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <unistd.h>

#include <iostream>
#include <fstream>

#include "faodel-common/Common.hh"
#include "opbox/OpBox.hh"

#include "Globals.hh"
#include "OpInterjobPing.hh"

//The Globals class just holds basic communication vars we use in these
//examples (ie mpi ranks, etc). It has a generic hook for starting/stopping
//all nodes in this mpi run to make the OpBox codes easier to understand.
Globals G;


//The FAODEL uses a plain-text configuration string to set
//different parameters in the stack. The below string defines what
//port the whookie sever will listen on, the type of directory management
//service to employ, and whether internal components should spew their
//debug information or not. If things don't work, try turning on the debug
//info to get a better idea of where things are breaking

std::string default_config_string = R"EOF(
# insert your own settings here or place in a file pointed to by env var FAODEL_CONFIG
)EOF";

using namespace std;


std::vector<faodel::nodeid_t> get_server_nodeids(faodel::Configuration &config)
{
    //determine the filename for the server nodeids
    std::string nodeid_file;
    config.GetString(&nodeid_file, "job2job.nodeid_file", "server_nodeids");

    //add the server nodeids to the Configuration
    config.AppendFromFile(nodeid_file);

    int64_t server_count;
    config.GetInt(&server_count, "job2job.server_count", "0");
    std::string server_nodeid_list;
    config.GetString(&server_nodeid_list, "job2job.server_nodeids", "");

    std::vector<std::string> urls = faodel::Split(server_nodeid_list, ',');

    std::vector<faodel::nodeid_t> nodeids;
    for (int i=0;i<urls.size();i++) {
        nodeids.push_back(faodel::nodeid_t(urls[i]));
    }

    return nodeids;
}


int main(int argc, char **argv){

  //This is a simple example of how to launch a small ping-pong communication
  cout <<"job2job client\n";

  //First, we need to register our new Op so OpBox will know how to handle a
  //particular Op whenever it sees a user request or message relating to it.
  //The RegisterOp template examines the class you give it by creating and
  //examining a dummy instance of the class to see what its id/name are.
  //
  //Ideally, we want to do registration *before* we start up opbox because
  //pre-init registrations can be maintained in a special list that doesn't
  //incur locking overheads at runtime. You *can* register ops after the
  //OpBox is started. However, those ops (1) incur a mutex and (2) won't be
  //known to OpBox until they're registered.
  opbox::RegisterOp<OpInterjobPing>();

  //Next, we want to create a master config that tells all the faodel
  //components how they should be configured. For the examples, the Global
  //class modifies the config so it can designate rank 0 as the master and
  //any other nodes as servers.
  faodel::Configuration config(default_config_string);
  config.AppendFromReferences();

  G.StartAll(argc, argv, config);

  MPI_Comm client_comm;
  int client_color=2;
  MPI_Comm_split(MPI_COMM_WORLD, client_color, G.mpi_rank, &client_comm);

  sleep(5);

  std::vector<faodel::nodeid_t> nodeids = get_server_nodeids(config);

  opbox::net::peer_ptr_t *peers = new opbox::net::peer_ptr_t[nodeids.size()];
  for (int i=0;i<nodeids.size();i++) {
      opbox::net::Connect(&peers[i], nodeids[i]);
  }

  for (int i=0;i<nodeids.size();i++) {
    OpInterjobPing *op1 = new OpInterjobPing(peers[i], "this is the first ping");
    future<string> fut1 = op1->GetFuture();
    opbox::LaunchOp(op1);

    string res1=fut1.get();
    cout<<"First result is '"<<res1<<"'\n";
  }

  //Finally, do an mpi barrier to sync all nodes and then invoke shutdown procedures
  //to stop the FAODEL. Global also does an mpi finalize to close out the test.
  G.StopAll();

  return 0;
}
