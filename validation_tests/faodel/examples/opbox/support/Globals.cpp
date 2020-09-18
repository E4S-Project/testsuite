// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <assert.h>

#include "faodel-common/Common.hh"
#include "Globals.hh"

using namespace std;
using namespace faodel;


Globals::Globals() : nodes(nullptr), peers(nullptr), debug_level(0) {}
Globals::~Globals(){
  if(nodes!=nullptr) delete[] nodes;
  if(peers!=nullptr) delete[] peers;
}


// This version looks at config to guess what the ip should be. The root info
// is bcast to all nodes, the config updated, and then bootstrap starts.
void Globals::StartAll(int &argc, char **argv, faodel::Configuration &config){

  stringstream ss;
  string whookie_port;

  int provided;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

  //Determine role based on our rank
  int mpi_dirroot_rank=0;
  config.Append("node_role", (mpi_rank==mpi_dirroot_rank) ? "master" : "server");

  //Add in debug messages for globals
  for(int i=1; i<argc; i++){
    if(string(argv[i])=="-v"){         debug_level=1;
    } else if(string(argv[i])=="-V"){  debug_level=2;
    }
  }

  log("Starting up.");

  nodeid_t dirroot_node;

  if(mpi_rank==mpi_dirroot_rank){
    //Mark ourself as dirroot
    config.Append("dirman.host_root true");

    //Startup the root node, so we can get its id
    bootstrap::Start(config, opbox::bootstrap);
    dirroot_node = opbox::GetMyID();
  }

  //Share the dirroot with everyone
  MPI_Bcast(&dirroot_node, sizeof(faodel::nodeid_t), MPI_CHAR, mpi_dirroot_rank, MPI_COMM_WORLD);


  if(mpi_rank!=mpi_dirroot_rank){
    //Store the root node here
    config.Append("dirman.root_node "+dirroot_node.GetHex());

    //Startup all the registered services
    bootstrap::Start(config, opbox::bootstrap);
  }

  //get info about everyone
  myid  = opbox::GetMyID();
  nodes = new faodel::nodeid_t[mpi_size];
  peers = new opbox::net::peer_ptr_t[mpi_size];


  ss.str(std::string());
  ss <<"Rank "<<mpi_rank<<" is "<<myid.GetHex()<<" with root of "<<dirroot_node.GetHex()<<endl;
  log(ss.str());

  //Share ids with everyone
  MPI_Allgather(&myid, sizeof(faodel::nodeid_t),MPI_CHAR,
                nodes, sizeof(faodel::nodeid_t),MPI_CHAR,
                MPI_COMM_WORLD);


  //Connect with everyone
  for(int i=0; i<mpi_size; i++){
    if(i==mpi_rank) peers[i]=nullptr;
    else{
      ss.str(std::string());
      ss << "Connecting to "<<opbox::net::GetMyID().GetHex()<<endl;
      log(ss.str());
      opbox::net::Connect(&peers[i], nodes[i]);
    }
  }
  dbg("Connected");
  //KHALT();
}

void Globals::StopAll(){

  MPI_Barrier(MPI_COMM_WORLD);

  bootstrap::Finish();
  MPI_Finalize();

}



void Globals::dump(){
  cout <<"Rank"<<mpi_rank<<" all_ids:";
  if(nodes!=nullptr){
    for(int i=0; i<mpi_size; i++)
      cout <<" "<<nodes[i].GetHex();
  }
  cout <<endl;
}
void Globals::log(string s){
  if(debug_level>0){
    cout<<"["<<mpi_rank<<"] "<<s<<endl;
  }
}
void Globals::dbg(string s){
  if(debug_level>1){
    cout<<"<"<<mpi_rank<<"> "<<s<<endl;
  }
}



