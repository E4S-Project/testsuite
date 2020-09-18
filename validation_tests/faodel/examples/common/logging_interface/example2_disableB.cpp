// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <iostream>

#include "ClassB.hh"

using namespace std;

void example2_disableB() {

  B b1("version1");

  cout <<"2.1 No messages should follow this. There should be no delays\n";
  b1.ConfigureLoggingDebug(true);
  b1.ConfigureLoggingInfo(true);
  b1.ConfigureLoggingWarn(true);
  b1.DoLoggingExamples();
  b1.DoDelayDump();

}
