// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#ifndef SERVICE_C_SERVICEC_HH
#define SERVICE_C_SERVICEC_HH

// Approach C: Require user to manually launch a bootstrap registration
// function at start and deregister at stop. This approach is meant
// to be a failsafe for when the runtime has other stuff in it that
// messes things up and we can't do any auto registration tricks.

// Advantages
// - Should work everywhere
// - Simple, details hidden
//
// Disadvantages
// - Not automatic, every program has to have the bootstraps
// - Global variable


#include <string>

namespace service_c {

void RegisterBootstrap();
void DeregisterBootstrap();

void doOp(std::string command);


} // namespace service_c

#endif // SERVICE_C_SERVICEC_HH
