// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#ifndef KELPIE_DIMSUM_PRODUCER_HH
#define KELPIE_DIMSUM_PRODUCER_HH

#include "WorkerThread.hh"
#include "DimSum.hh"

class Producer
  : public WorkerThread {

public:
  Producer(faodel::ResourceURL pool_url, int id, int num_producers, int num_timesteps);
  ~Producer();

  void Run();

private:
  DimSum dim_sum;

};

#endif // KELPIE_DIMSUM_PRODUCER_HH
