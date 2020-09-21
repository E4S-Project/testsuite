// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

// Example: opbox dirty use: Lingering Data
// Purpose: Lunasa manages memory for many components and uses lunasa data
//          objects (LDOs) to allow different components to reference the
//          memory. Cleaning up one's program is tedious, and often users
//          just want to exit without worrying about and references they
//          still hold. DW components must take this behavior into account
//          and guarantee the application won't segfault because of it.
//
//          This example demonstrates that OpBox and Lunasa behave properly
//          when a user stops the system without cleaning up app LDOs.
//
// Keypoints
//  - LDO Cleanup: Users can drop ownership of an LDO by assigning an empty LDO
//    - note: in future work, this should be more explict
//  - No Segfaults: A user can hold LDOs at exit and not have the lib segfault

#include <iostream>

#include <sstream>
#include <stdlib.h>
#include <string.h>

#include "faodel-common/Common.hh"
#include "lunasa/Lunasa.hh"
#include "opbox/OpBox.hh"


#include "Globals.hh"


using namespace std;


//Globals holds mpi info and manages connections (see ping example for info)
Globals G;

//The configuration used in this example (see ping example for info)
std::string default_config_string = R"EOF(
# Note: node_role is defined when we determine if this is a client or a server
net.transport.name    mpi
master.whookie.port   7777
server.whookie.port   1991

dirman.type           centralized
dirman.root_role      master

#bootstrap.debug true
#whookie.debug   true
#opbox.debug     true
#dirman.debug    true
lunasa.debug true

)EOF";

int main(int argc, char **argv){

  //Create an empty ldo handle. This isn't assigned to anything yet and has reference count of 0
  lunasa::DataObject ldo;

  //Start up everything, including lunasa
  faodel::Configuration config(default_config_string);
  G.StartAll(argc,argv, config);

  //Allocate an ldo of 1k from eager memory
  ldo = lunasa::DataObject(0, 1000, lunasa::DataObject::AllocatorType::eager);

  //See if we're a good user
  bool cleanup=false;
  for(int i=1; i<argc;i++){
    if((string(argv[i]) == "--cleanup")||(string(argv[i])=="-c"))
      cleanup=true;
  }

  //Good user will wipe out all ldo's before calling shutdown
  if(cleanup){
    //Currently, only way to wipe out ldo is assign it an empty entry
    //We really should have a Release() function that signifies we're done
    //with the item.
    cout <<"Droping Reference to LDO when refcount is "<<ldo.internal_use_only.GetRefCount()<<endl;
    ldo = lunasa::DataObject();
  }

  G.StopAll();
  cout <<"Done. Exiting\n";

  //If user didn't get rid of the ldo, we should insert a panic message here,
  //but not segfault.

  return 0;
}
