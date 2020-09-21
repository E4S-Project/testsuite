// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

// Example: kelpie using-whookie
// Purpose: Whookie provides an easy way to check on the status of different
//          components in the FAODEL stack. This example shows how you can use
//          a built-in kelpie whookie to peek at the status of the localkv.
//
// Keypoints:
//  - Built-In: Whookie is built into the stack, users just need to locate it
//  - Handles: The Kelpie whookie handles let you peek into Kelpie's state
//    - /kelpie/lkv: Shows you info about local data objects
//    - /kelpie/pool_registry: Shows you how many Pool handles are in use

#include <iostream>

#include "kelpie/Kelpie.hh"
#include "whookie/Server.hh"

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

)EOF";

using namespace std;

int main(){

  char tmp;
  cout <<"Kelpie using-whookie example\n";

  //Startup all the registered services
  faodel::Configuration config(default_config_string);
  faodel::bootstrap::Start(config, kelpie::bootstrap);

  //Retrieve info about our built-in webserver
  string whookie_url = whookie::Server::GetNodeID().GetHttpLink();

  cout <<"Kelpie is now up and running.\n"
       <<"   Go to a browser and look at: "<<whookie_url<<"\n"
       <<"Press [enter]\n";

  cin.get();

  cout <<"Kelpie's local kv should be empty at this time. Whookie has a stats page for it\n"
       <<"   under /kelpie/lkv: "<<whookie_url<<"/kelpie/lkv&detail\n"
       <<"Press [enter]\n";

  cin.get();

  cout <<"Generating some fake row/column data and publishing to kelpie\n";

  //Get a handle to the lkv and insert some data objects into it
  kelpie::Pool lkv1 = kelpie::Connect("local:");
  kelpie::Pool lkv2 = kelpie::Connect("local:[0x2112]");
  for(auto name : {"bob","jane","frank"}){
    for(int i=0; i<string(name).size(); i++){ //the length here is just to make cols different
      lunasa::DataObject ldo(0, (i+1)*100, lunasa::DataObject::AllocatorType::eager);
      kelpie::Key key(name, to_string(i));
      lkv1.Publish(key, ldo);
      lkv2.Publish(key, ldo);
    }
    cout <<"  Published row '"<<name<<"' with "<<string(name).size()<<" columns in it\n";
  }

  //Block until the user wants to move on
  cout <<"\nIf you reload the webpage, you should now see multiple rows in kelpie.\n"
       <<"Press [enter]\n";

  cin.get();

  //Encourage users to also look at pool_registry
  cout <<"\nIf you look at /kelpie/pool_registry, you can see what Pool handles\n"
       <<"   are known: "<<whookie_url<<"/kelpie/pool_registry\n"
       <<"Press [enter]\n";
  cin.get();


  //Close out
  faodel::bootstrap::Finish();
  cout <<"Done.\n";

  return 0;
}
