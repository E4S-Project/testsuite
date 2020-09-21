// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#ifndef SIMPLEDATASTORE_HH
#define SIMPLEDATASTORE_HH

#include "lunasa/Lunasa.hh"
#include "lunasa/DataObject.hh"


namespace examples {

namespace SimpleDataStore {

//These are the manual interfaces if you want to start the service on
//its own. Bootstrap automates this process and should be easier to use.
void Init(const faodel::Configuration &config);
void Start();
void Finish();

void Add(std::string key, lunasa::DataObject value);
lunasa::DataObject Get(std::string key);
void Remove(std::string key);

} // namespace SimpleDataStore

} // namespace examples

#endif // SIMPLEDATASTORE_HH
