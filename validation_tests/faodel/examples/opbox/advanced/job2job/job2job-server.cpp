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


void write_server_urls(MPI_Comm c, faodel::Configuration &config)
{
    int server_count;
    faodel::nodeid_t *server_nodes;

    MPI_Comm_size(c, &server_count);
    server_nodes = new faodel::nodeid_t[server_count];
    MPI_Allgather(&G.myid, sizeof(faodel::nodeid_t),MPI_CHAR,
                  server_nodes, sizeof(faodel::nodeid_t),MPI_CHAR,
                  c);

    //create a Configuration fragment for the server nodeids
    stringstream ss;
    ss<<"job2job.server_count          "<<server_count<<std::endl;
    ss<<"job2job.server_nodeids        "<<server_nodes[0].GetHex();
    for(int i=1; i<server_count; i++){
        ss<<","<<server_nodes[i].GetHex();
    }
    ss<<std::endl;

    //determine the filename for the server nodeids
    std::string nodeid_file;
    config.GetString(&nodeid_file, "job2job.nodeid_file", "server_nodeids");

    //write the Configuration fragment to the server nodeid file
    ofstream nodeid_ofs(nodeid_file);
    nodeid_ofs<<ss.str();
    nodeid_ofs.close();
}

int main(int argc, char **argv){

  //This is a simple example of how to launch a small ping-pong communication
  cout <<"job2job server\n";

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

  MPI_Comm server_comm;
  int server_color=1;
  MPI_Comm_split(MPI_COMM_WORLD, server_color, G.mpi_rank, &server_comm);

  write_server_urls(server_comm, config);

  //Sleeping is lame.  You are better than that.
  sleep(30);

  //Finally, do an mpi barrier to sync all nodes and then invoke shutdown procedures
  //to stop the FAODEL. Global also does an mpi finalize to close out the test.
  G.StopAll();

  return 0;
}
