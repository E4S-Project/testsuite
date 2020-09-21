// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

// Example: kelpie start-finish
// Purpose: This is a minimal example that demonstrates how to start/finish
//          Kelpie using a basic configuration. It runs as a standalone
//          app and does not need to be launched with mpi.
//
// Keypoints
//  - Configs: Use a multi-line string to pass config info into Kelpie
//    - Transports: Set nnti to use MPI if running on a standalone laptop
//    - CONFIG: Additional config info can be imported through env vars
//    - Debugs: Debug info can be toggled for individual components
//    - nonet: Tell Kelpie to run w/ nonet when doing standalone tests
//  - Bootstrap: Pass the config to bootstrap to configure all components
//    - Start(config): configures and starts all components in order
//    - Finish(): stops all components in reverse order

#include <iostream>

#include "kelpie/Kelpie.hh"


//The configuration used in this example
std::string default_config_string = R"EOF(

# For local testing, tell kelpie to use the nonet implementation
kelpie.type nonet

# Uncomment these options to get debug info for each component
#bootstrap.debug true
#whookie.debug   true
#opbox.debug     true
#dirman.debug    true
#kelpie.debug    true

# Note: You can put additional config info like the above in an
#       external text file and have kelpie load it on boot
#       just by setting the environment variable FAODEL_CONFIG
#       to the filename.

)EOF";

using namespace std;

int main(){

  cout <<"Kelpie Start/Finish example\n";

  //Startup all the registered services
  faodel::Configuration config(default_config_string);
  faodel::bootstrap::Start(config, kelpie::bootstrap);

  cout <<"Started..\n";

  //Insert your own magic

  cout <<"Finishing..\n";
  faodel::bootstrap::Finish();

  cout <<"Done.\n";

  return 0;
}
