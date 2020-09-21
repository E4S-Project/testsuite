// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#ifndef KELPIE_DIMSUM_DIMSUM_HH
#define KELPIE_DIMSUM_DIMSUM_HH

#include "kelpie/Kelpie.hh"

class DimSum {

public:
  DimSum(faodel::ResourceURL pool_url, int num_producers);
  ~DimSum();

  void WriterInitialize(int producer_id);
  void WriterStartTimestep(int timestep, int max_vals);
  int  WriterAppend(int value);
  void WriterFinishTimestep();

  int ReaderGetTimestep(int timestep, int **vals, int *num_vals);

private:

  int producer_id;
  int num_producers;
  kelpie::Pool pool;

  kelpie::Key current_key;
  lunasa::DataObject ldo_current;

  int writer_max_vals;
  int writer_num_vals;
  int *writer_data;

};


#endif // KELPIE_DIMSUM_DIMSUM_HH
