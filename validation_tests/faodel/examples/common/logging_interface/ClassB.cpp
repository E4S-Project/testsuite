// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include "ClassB.hh"

#include <unistd.h>

//Expect a user to set a flag to disable logging at build time via cmake
#define DISABLE_CLASS_B_LOGGING 1


//For this implementation file only, we disable logging by
// 1. Setting LOGGING_DISABLED to a nonzero value
// 2. Loading the LoggingInterfaceMacros, which remove dbg(), info(), warn()
#define LOGGING_DISABLED (DISABLE_CLASS_B_LOGGING)
#include "faodel-common/LoggingInterfaceMacros.hh"



//Rest of file is same as Class A


B::B(std::string name) :  LoggingInterface("B."+name) {
}

void B::DoConfig(faodel::Configuration &config) {
  //We can pull all our settings from a config. However, this option is
  //protected, since we expect the component to take care of itself (and not the parent)
  ConfigureLogging(config);
}

//This provides an example of all the log functions
void B::DoLoggingExamples() {
  dbg("This is a debug message");
  info("This is an info message");
  warn("This is a warn message");
}

void B::DoDelayDump() {
  //Even though the user can disable a logger, the calling
  //code will still assemble the input string and do any
  //operations you embed in it. That's why you want to use
  //the compile-time option to disable logging if you're doing
  //a performance run.
  dbg("This has a delay in it "+to_string(sleep(5))+" here");
}
