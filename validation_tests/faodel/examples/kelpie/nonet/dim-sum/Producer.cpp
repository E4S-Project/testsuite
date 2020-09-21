// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <iostream>
#include <chrono>
#include <cstdlib>

#include "Producer.hh"

using namespace std;

Producer::Producer(faodel::ResourceURL pool_url, int id, int num_producers, int num_timesteps)
  : WorkerThread(id, num_timesteps),
    dim_sum(pool_url, num_producers) {

}
Producer::~Producer(){
}

void Producer::Run(){

  dim_sum.WriterInitialize(id);

  int max_vals = 1000;
  for(int t=0; t<num_timesteps; t++){

    int num_items=(rand()%max_vals)+1;

    dim_sum.WriterStartTimestep(t, max_vals);

    for(int i=0; i<num_items; i++)
      dim_sum.WriterAppend( (rand()%256)+1 );

    dim_sum.WriterFinishTimestep();

    cout <<"Producer "<<id<<" at timestep "<<t<<" generated "<<num_items<<endl;

    //Put in some random delay
    std::this_thread::sleep_for(std::chrono::milliseconds((rand()%2000)+1));
  }

}
