// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

// Singleton Example
//
// Bootstrap was originally designed so you could define a bunch
// of different singletons and some static trickery would automatically
// register them all with bootstrap. This example shows three techniques
// for registering the singleton services:
//
//  ServiceA: A hidden global variable for a class that registers itself
//  ServiceB: A static private member holds a class that registers itself
//  ServiceC: Singleton is created the first time a function is called
//
// Note: We NO LONGER RECOMMEND using this automatic service registration
//       for singleton services. Linkers often discard the singleton code
//       during optimization unless you wrap the link objects with
//       some -Wl,--whole-archive flags.
//
// Instead: We recommend doing dependency injection and manually register
//          each service.


#include <iostream>
#include "faodel-common/Common.hh"

#include "ServiceA.hh"
#include "ServiceB.hh"
#include "ServiceC.hh"


using namespace std;


string fn_no_components() {
  return "";
}

int main(int argc, char *argv[]) {


  service_c::RegisterBootstrap(); //Manually create/register entity


  //We need to start the services, but in this case we don't have
  //a top-level bootstrap that needs to be registered. We can either
  //declare the empty registration in a well-named function, or just
  //use an inline lambda.
  faodel::bootstrap::Start(faodel::Configuration(), []() { return"";} );
  //faodel::bootstrap::Start(faodel::Configuration(), fn_no_components );



  service_a::doOp("Foobar");

  service_b::ServiceB b1;
  service_b::ServiceB b2;
  b1.doOp("From b1");
  b2.doOp("From b2");

  service_c::doOp("Stuff");


  faodel::bootstrap::Finish();

  service_c::DeregisterBootstrap(); //Manually deallocates entity


  return 0;
}
