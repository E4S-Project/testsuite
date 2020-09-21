// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#ifndef SERVICE_B_SERVICEB_HH
#define SERVICE_B_SERVICEB_HH

#include <string>

// Approach B: Use a static implementation inside of a class to hold our
// singleton. User is provided a simple pimpl-ish class that they can
// use as many times as they'd like.
//
// Advantages:
// - bootstrap details hidden from user
// - automatically registers w/ bootstrap
// - No global var (hidden inside static)
//
// Disadvantages:
// - Extra wrapping: Class for a class.
// - Registration may break in things like charm++
// - Api can be redundant:  service_b::ServiceB.command().

namespace service_b {

class ServiceB_Impl; //Forward Ref

class ServiceB {
public:
  ServiceB();
  ~ServiceB();

  void doOp(std::string command);

private:
  static ServiceB_Impl impl;
};


} // namespace service_b

#endif // SERVICE_B_SERVICEB_HH
