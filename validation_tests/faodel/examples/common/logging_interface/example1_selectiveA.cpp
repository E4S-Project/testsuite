// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <iostream>

#include "ClassA.hh"

using namespace std;

void example1_selectiveA() {

  //Create two new components of A. A's ctor was built knowing there might be
  //multiple As, so it asks for an additional label to append to the
  //component name. In this case, the ctor uses the name "A."+name.
  A a1("version1"); //ctor will call this component A.version1
  A a2("version2"); //ctor will call this component A.version2


  //We can turn logging on/off for different types of messages. Initially
  //a component starts up with no logging enabled.

  //Initially this should not print any messages
  cout <<"1.1 No messages should follow this.\n";
  a1.DoLoggingExamples();
  a2.DoLoggingExamples();


  //We can toggle a component's logging inside and out of the component. This
  //test turns on one type for each of the two instances.
  cout <<"1.2 Should only have logging for a1 debug, a2 warn\n";
  a1.ConfigureLoggingDebug(true);
  a2.ConfigureLoggingWarn(true);
  a1.DoLoggingExamples();
  a2.DoLoggingExamples();

  //It's easier just to stuff the logging info into the configuration you
  //provide at Init(). ConfigureLogging is something usually done inside
  //the component, and is therefore protected.
  //
  //NOTE: Configuration names are converted to lowercase.
  cout <<"1.3 Setting by Configuration. a1 info and a2 debug on\n";
  faodel::Configuration config1;
  config1.Set("A.version1.log.info",  true);
  config1.Set("A.version2.log.debug", true);
  a1.DoConfig(config1);
  a2.DoConfig(config1);
  a1.DoLoggingExamples();
  a2.DoLoggingExamples();

  cout <<"1.4 Disable logs and doing a command with delay in it.\n"
       <<"    We expect no output, but for the program to stall for 5 seconds\n";
  a1.ConfigureLoggingDebug(false);
  a1.ConfigureLoggingInfo(false);
  a1.ConfigureLoggingWarn(false);
  a1.DoDelayDump();

}
