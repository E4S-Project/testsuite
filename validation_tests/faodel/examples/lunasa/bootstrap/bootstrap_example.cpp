// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <iostream>
#include <assert.h>

#include "faodel-common/Common.hh"
#include "lunasa/Lunasa.hh"
#include "lunasa/DataObject.hh"

using namespace std;



int main(){
  cout <<"Starting Bootstrap example's main()\n";

  //FAODEL's Configuration allows us to describe configuration info for
  //different components in plain text. A Configuration object holds
  //values in a simple key/value notation that the components can
  //pull out.
  faodel::Configuration config("");

  //There are two steps to initializing the components: Init and 
  //Start. The Init sets up basic configuration, but leaves the
  //component in a state where later components can update its 
  //settings. The Start option is used to officially start the
  //component. After this point the service cannot be modified.
  faodel::bootstrap::Start(config, lunasa::bootstrap);


  {
    cout <<"Start done\n";
    lunasa::DataObject ldo(0, 1024*sizeof(int), lunasa::DataObject::AllocatorType::eager);
    cout <<"alloc done\n";
    //cout <<"Capacity is "<<ldo.capacity()<<endl;

    int *x = static_cast<int *>(ldo.GetDataPtr());
    assert(x!=nullptr);
    cout <<"cast\n";
    for(int i = 0; i<1; i++){
      cout <<"Writing "<<i<<endl;
      x[i] = i;
    }
    cout <<"Work done\n";
  }


  //When all work is done, call the Finish command. This stops all
  //components and deallocates space.
  faodel::bootstrap::Finish();

  cout <<"Exiting Bootstrap example\n";
}
