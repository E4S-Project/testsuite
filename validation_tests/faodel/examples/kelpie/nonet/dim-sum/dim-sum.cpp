// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

// Example: kelpie local pool basics
// Purpose:
//
// Keypoints


#include <iostream>

#include "kelpie/Kelpie.hh"

#include "Producer.hh"
#include "Consumer.hh"


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

  const int num_producers  = 2;
  const int num_consumers  = 2;
  const int num_timesteps  = 100;

  faodel::ResourceURL url("local:");

  cout <<"Kelpie dim-sum Data Interface Module example\n";

  //Startup all the registered services
  faodel::Configuration config(default_config_string);
  faodel::bootstrap::Start(config, kelpie::bootstrap);

  //Create Generators
  vector<WorkerThread *> workers;
  for(int i=0; i<num_producers; i++){ workers.push_back( new Producer(url, i, num_producers, num_timesteps) ); }
  for(int i=0; i<num_consumers; i++){ workers.push_back( new Consumer(url, i, num_producers, num_consumers, num_timesteps) ); }

  for(auto &wptr : workers){
    wptr->Start();
  }

  //Wait for all to finish
  for(auto &wptr : workers){
    wptr->Join();
  }




  cout <<"Finishing..\n";
  faodel::bootstrap::Finish();

  cout <<"Done.\n";

  return 0;
}
