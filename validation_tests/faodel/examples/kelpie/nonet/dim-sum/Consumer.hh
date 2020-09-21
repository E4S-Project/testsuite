// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#ifndef KELPIE_DIMSUM_CONSUMER_HH
#define KELPIE_DIMSUM_CONSUMER_HH

#include "WorkerThread.hh"
#include "DimSum.hh"

class Consumer
  : public WorkerThread {

public:
  Consumer(faodel::ResourceURL pool_url, int id, int num_producers, int num_consumers, int num_timesteps);
  ~Consumer();

  void Run();

private:
  DimSum dim_sum;
  int num_consumers;
};

#endif // KELPIE_DIMSUM_CONSUMER_HH
