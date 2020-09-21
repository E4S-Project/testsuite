// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

// Killit Example
//
// In addition to providing status information, Whookie can be used as a way
// to trigger actions in a remote application. This example shows how to add
// a hook that shuts the server down.

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <thread>

#include "whookie/Server.hh"
#include "faodel-common/QuickHTML.hh"

#include "faodel-common/Common.hh"

using namespace std;

//Standard configuration settings
string default_config = R"EOF(
whookie.port 2112

bootstrap.debug true
whookie.debug true
)EOF";

bool keep_going=true;

void ShutMeDown(){
  cout<<"Received shutdown request\n";
  faodel::bootstrap::Finish();
  keep_going=false;
}

int main(int argc, char* argv[]) {

 
  //Register a simple killit page that calls the ShutMeDown function
  whookie::Server::registerHook("/killit", [] (const map<string,string> &args, stringstream &results){
      ShutMeDown();
    });


  //Startup bootstraps (should only be whookie)
  faodel::bootstrap::Init(faodel::Configuration(default_config),
                           whookie::bootstrap);

  //Once it's started, you can retrieve our node id
  faodel::nodeid_t nid = whookie::Server::GetNodeID();
  cout<<"Started Webserver. Go to killit page to kill it: curl "<<nid.GetHttpLink("/killit")<<endl;

  //Busy loop. Webserver is in another thread.
  do {
    this_thread::sleep_for(chrono::seconds(1));
  } while(keep_going);

  cout <<"Done. Exiting\n";
 
  return 0;
}
