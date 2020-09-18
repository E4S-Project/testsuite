// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <unistd.h>
#include <iostream>
#include <atomic>

#include "faodel-common/Configuration.hh"
#include "faodel-common/Bootstrap.hh"

#include "faodel-services/BackBurner.hh"

using namespace std;
using namespace faodel;


int main() {

  atomic<int> global_count(0);

  //Tell bootstrap that we only want to run the backburner service (and anything it depends on)
  bootstrap::Start(faodel::Configuration(), backburner::bootstrap);

  //Launch a set of simple tasks, one by one. Each insert requires a lock operation.
  for(int i=0; i<10; i++) {
    cout <<"Inserting "<<i<<endl;
    backburner::AddWork( [i,&global_count] () { std::cout << "Single Op is "<<i<<endl; global_count++; return 0;} );
  }

  //If you need to launch a bundle of jobs, you can stuff them in a vector. This can
  //minimize locking costs for both this thread and the backburner thread.
  vector<fn_backburner_work> jobs;
  for(int i=0; i<10; i++) {
    jobs.push_back( [i,&global_count] () { std::cout << "Bundled Op is "<<i<<endl; global_count++;  return 0;} );
  }
  cout <<"Inserting bundle of jobs\n";
  backburner::AddWork(jobs);


  //This thread waits until we see all ops have finished
  cout<<"All dispatched. Waiting for all ops to finish\n";
  while(global_count!=20)
    sleep(1);

  cout <<"Global event count is now: "<<global_count<<endl;

  bootstrap::Finish();
  return 0;
}
