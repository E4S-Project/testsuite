// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

// A more useful thing to do with the mpisyncstart service is use it to update
// a config with runtime info. This service can currently perform two
// updates for dirman resources:
//
// dirman.root_node_mpi rank ==> dirman.root_node whookie_nodeid
//    Often you want to designate a certain rank in a job as the root node
//    for the dirman service, but you don't know what the nodeid is until
//    you start. If the mpisyncstart service sees the dirman.root_node_mpi
//    variable, all nodes will wait for an mpi bcast of the root node's
//    nodeid. They will then plug the dirnan.root_node into the config
//    so that the dirman service will be ready to use it.
//
// dirman.resources_mpi[] myurl 1-4 ==> dirman.resources[] myurl&AG0=0x1&AG1=0x2...
//    You may also want to preload dirman with some resources (eg
//    a DHT with all nodes). For each dirman.resources_mpi resource that
//    is found, the root node will translate all the ranks into nodes.
//    in order to do this the nodes must all exchange their node ids.
//    TODO: While mpisync does the translation, dirman preloading has not been implemented yet
//

#include <iostream>
#include <mpi.h>

#include "faodel-common/Common.hh"
#include "faodel-common/Bootstrap.hh"
#include "faodel-services/MPISyncStart.hh"

using namespace std;
using namespace faodel;

//This service just dumps the configuration that is handed to it
class ConfigDump
        : public faodel::bootstrap::BootstrapInterface {

public:
  //Bootstrap API
  void Init(const faodel::Configuration &config) override {
    cout <<"ConfigDump received the following config:\n"
         <<config.str();
  }
  void Start() override {}
  void Finish() override {}
  void GetBootstrapDependencies(std::string &name,
                                std::vector<std::string> &requires,
                                std::vector<std::string> &optional) const override {
    name="configdump";
    requires={"mpisyncstart"};
    optional={};
  }
};

//Bootstrap for this service
string config_dump_bootstrap() {
  static ConfigDump singleton;

  faodel::mpisyncstart::bootstrap();
  faodel::bootstrap::RegisterComponent(&singleton);
  return "ConfigDump"; //<--Note: You MUST return the name of the last component
}



int main(int argc, char **argv) {

  //Note: you MUST initialize mpi before starting this service
  int mpi_rank, mpi_size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

  faodel::Configuration config;
  config.Append("mpisyncstart.debug",    "true");
  config.Append("dirman.root_node_mpi",  "0");

  //Running this should cause the mpisyncstart to translate the
  //dirman,root_node_mpi rank to a whookie nodeid. You should
  //see the update when you
  faodel::bootstrap::Start(config, config_dump_bootstrap);
  faodel::bootstrap::Finish();

  MPI_Finalize();

}
