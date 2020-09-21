// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <assert.h>

#include "SimpleDataStore.hh"

using namespace std;

namespace examples {

namespace SimpleDataStore {

namespace internal {

std::map<std::string, lunasa::DataObject> store;

} // namespace internal

//Bootstraps
void Init(const faodel::Configuration &config) {  return; }
void Start()                                    {  return; }
void Finish()                                   {  return; }

void Add(std::string key, lunasa::DataObject value) { internal::store[key] = value; }
lunasa::DataObject Get(std::string key)             { return internal::store[key]; }
void Remove(std::string key)                        { internal::store.erase(key); }

} // namespace SimpleDataStore

} // namespace examples
