// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <iostream>
#include <exception>
#include <cstring>

#include "DimSum.hh"

using namespace std;

DimSum::DimSum(faodel::ResourceURL pool_url, int num_producers)
  : producer_id(-1), num_producers(num_producers) {

  pool = kelpie::Connect(pool_url);

}
DimSum::~DimSum(){
}

void DimSum::WriterInitialize(int producer_id){
  this->producer_id = producer_id;
}

void DimSum::WriterStartTimestep(int timestep, int max_vals){

  current_key.TemplatedK1<int>(timestep);
  current_key.TemplatedK2<int>(producer_id);

  ldo_current = std::move(lunasa::DataObject(max_vals*sizeof(int)));
  writer_max_vals = max_vals;
  writer_num_vals = 0;
  writer_data = (int *)ldo_current.GetDataPtr();
}

int DimSum::WriterAppend(int value){
  if(writer_num_vals >= writer_max_vals){
    throw std::overflow_error("Unknown case condition");
  }

  writer_data[writer_num_vals] = value;
  writer_num_vals++;
  return (writer_num_vals == writer_max_vals);
}

void DimSum::WriterFinishTimestep(){
  //Resize: currently disabled. will be refactored in next release
  //ldo_current.Resize(writer_num_vals * sizeof(uint32_t));
  pool.Publish(current_key, ldo_current);
}

int DimSum::ReaderGetTimestep(int timestep, int **vals, int *num_vals){
  vector<lunasa::DataObject> ldos;
  int total_bytes=0;

  //Fetch all items
  kelpie::Key key;
  key.TemplatedK1(timestep);
  for(int i=0; i<num_producers; i++){
    key.TemplatedK2(i);
    lunasa::DataObject ldo;

    int rc = pool.Need(key, &ldo);
    if(rc!=0) cerr <<"Need() could not get "<<key.str()<<endl;

    total_bytes += ldo.GetDataSize();
    ldos.push_back(ldo);
  }

  int *tmp = new int[total_bytes/sizeof(int)];

  //Copy all the blocks into an array for users
  int int_offset=0;
  for(auto &ldo : ldos){
    memcpy(&tmp[int_offset], ldo.GetDataPtr(), ldo.GetDataSize());
    int_offset += ldo.GetDataSize()/sizeof(int);
  }
  //Pass back the result
  *vals = tmp;
  *num_vals = int_offset;
  return 0;
}
