// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <unistd.h>

#include "ClassA.hh"

A::A(std::string name) :  LoggingInterface("A."+name) {
}

void A::DoConfig(faodel::Configuration &config) {
  //We can pull all our settings from a config. However, this option is
  //protected, since we expect the component to take care of itself (and not the parent)
  ConfigureLogging(config);
}

//This provides an example of all the log functions
void A::DoLoggingExamples() {
  dbg("This is a debug message");
  info("This is an info message");
  warn("This is a warn message");
}

void A::DoDelayDump() {
  dbg("This has a delay in it "+to_string(sleep(5))+" here");
}
