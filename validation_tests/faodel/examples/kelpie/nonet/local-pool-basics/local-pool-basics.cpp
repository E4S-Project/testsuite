// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

// Example: kelpie local pool basics
// Purpose:
//
// Keypoints


#include <iostream>

#include "kelpie/Kelpie.hh"


//The configuration used in this example
std::string default_config_string = R"EOF(

# For local testing, tell kelpie to use the nonet implementation
kelpie.type standard

# Uncomment these options to get debug info for each component
#bootstrap.debug true
#whookie.debug   true
#opbox.debug     true
#dirman.debug    true
#kelpie.debug    true

)EOF";

extern void ex1_get_pool_handle();
extern void ex2_publish_and_need();

using namespace std;

int main(){

  cout <<"Kelpie Local Pool Basics example\n";

  //Startup all the registered services
  faodel::Configuration config(default_config_string);
  faodel::bootstrap::Start(config, kelpie::bootstrap);

  cout <<"Started..\n";

  ex1_get_pool_handle();
  ex2_publish_and_need();


  cout <<"Finishing..\n";
  faodel::bootstrap::Finish();

  cout <<"Done.\n";

  return 0;
}
