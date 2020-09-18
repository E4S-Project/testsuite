// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <iostream>
#include <unistd.h>
#include <assert.h>

#include "faodel-common/Common.hh"
#include "opbox/OpBox.hh"

#include "dirman/DirMan.hh"


#include "../../support/Globals.hh"


//The Globals class just holds basic communication vars we use in these
//examples (ie mpi ranks, etc). It has a generic hook for starting/stopping
//all nodes in this mpi run to make the OpBox codes easier to understand.
Globals G;



std::string default_config_string = R"EOF(
# Select a transport to use for nnti (laptop tries ib if not forced to mpi)
net.transport.name   mpi

# Put the 'master' node on a separate port so it won't get bumped around by
# the others on a single-node multi-rank run.
#
# note: node_role is set by Globals based on rank.
#
master.whookie.port   7777
server.whookie.port   1992

# Select the type of dirman to use. Currently we only have centralized, which
# just sticks all the directory info on one node (called root). We use roles
# to designate which node is actually the root.
dirman.type           centralized
dirman.root_role      master

# Turn these on if you want to see more debug messages
#bootstrap.debug           true
#whookie.debug             true
#opbox.debug               true
#dirman.debug              true
#dirman.cache.others.debug true
#dirman.cache.mine.debug   true

)EOF";

using namespace std;
using namespace faodel;
using namespace opbox;

//All the examples (simpler than dealing with headers)
void example1_plain_messages();
void example1_body_messages();
void example1_complex_body_messages();

void example2_ldo_messages();
void example2_ldo_string_messages();

void example3_ldo_string_request_reply();

void example4_boost_messages();

#ifdef USE_FLATBUFFERS
void example5_flatbuffer_messages();
#endif

#ifdef USE_CAPNPROTO
void example6_capnproto_messages();
#endif

#ifdef USE_CEREAL
void example7_cereal_messages();
#endif

int main(int argc, char **argv){

  faodel::Configuration config(default_config_string);
  config.AppendFromReferences();

  G.StartAll(argc, argv, config); //Normally, opbox::bootstrap would go here

  example1_plain_messages();
  example1_body_messages();
  example1_complex_body_messages();

  example2_ldo_messages();
  example2_ldo_string_messages();

  example3_ldo_string_request_reply();

  example4_boost_messages();

#ifdef USE_FLATBUFFERS
  example5_flatbuffer_messages();
#endif

#ifdef USE_CAPNPROTO
  example6_capnproto_messages();
#endif

#ifdef USE_CEREAL
  example7_cereal_messages();
#endif
  
  
  G.StopAll();

  return 0;
}
