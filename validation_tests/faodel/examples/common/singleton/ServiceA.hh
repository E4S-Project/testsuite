// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#ifndef SERVICE_A_SERVICEA_HH
#define SERVICE_A_SERVICEA_HH


// Approach A: Use a (hidden) global variable in the implementation file
// to hold a singleton. Use functions in a namespace to provide a simple
// api for working with the singleton.
//
// Advantages:
// - simple, to the point api calls (eg service_a::doOp())
// - bootstrap details hidden from user
// - automatically registers w/ bootstrap
//
// Disadvantages:
// - Global variable: doesn't feel c++-ish
// - Registration may break in things like charm++




#include "faodel-common/Common.hh"

namespace service_a {

//Top-level API for service a
void doOp(std::string command);

} // namespace service_a

#endif // SERVICE_A_SERVICEA_HH
