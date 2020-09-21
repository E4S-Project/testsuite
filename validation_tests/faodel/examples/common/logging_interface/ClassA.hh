// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#ifndef CLASSA_HH
#define CLASSA_HH

#include <iostream>
#include <string>
#include <vector>

#include "faodel-common/Common.hh"

using namespace std;


class A : public faodel::LoggingInterface {
public:
  A(std::string name);

  void DoConfig(faodel::Configuration &config);

  //This provides an example of all the log functions
  void DoLoggingExamples();

  void DoDelayDump();

};




#endif
