// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

// logging_interface example
//
// This LoggingInterface is a simple way for us to plug logging operations into
// classes. In order to use it, you need to provide a name for the
// component you're working with, as well as some settings for what you want
// logged.
//
// Note: Logging can be completely disabled at compile time by setting
//       setting the LOGGING_DISABLED flag in build tools.

#include "ClassA.hh"

extern void example1_selectiveA();
extern void example2_disableB();


int main(int argc, char* argv[]) {

  //Run a normal example, where user wants to control what components dump data
  example1_selectiveA();

  //Run an example where we want to remove all logging at compile time
  example2_disableB();

  return 0;
}
