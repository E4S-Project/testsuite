// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <iostream>
#include <chrono>

#include <assert.h>

#include "Consumer.hh"

using namespace std;

Consumer::Consumer(faodel::ResourceURL pool_url, int id, int num_producers, int num_consumers, int num_timesteps)
  : WorkerThread(id, num_timesteps),
    dim_sum(pool_url, num_producers),
    num_consumers(num_consumers) {

}
Consumer::~Consumer(){
}

void Consumer::Run(){

  int sum=0;

  //Each consumer works on its own timestep
  for(int t=id; t<num_timesteps; t+=num_consumers){
    int ts_sum=0;

    int *vals;
    int num_vals;

    int rc = dim_sum.ReaderGetTimestep(t, &vals, &num_vals);
    assert(rc==0);
    for(int i=0; i<num_vals; i++){
      ts_sum+=vals[i];
    }
    delete[] vals;
    sum+=ts_sum;

    cout <<"Consumer "<<id<<" at timestep "<<t<<". TsSum: "<<ts_sum<<" Sum: "<<sum<<endl;
    std::this_thread::sleep_for(std::chrono::milliseconds((rand()%2000)+1));
  }

}
