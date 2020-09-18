// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

// Boostrap Example
//
// Bootstrap is a way to establish the order in which different services
// are initialized, started up, and shutdown in FAODEL. The most direct way to
// create a bootstrap service is to create a singleton class with a
// BootstrapInterface. This interface has 5 functions:
//
//  1.  Init(const configuration): Most services just need to parse a config
//      in order to do their initialization. This function handles all the
//      initialization work
//  2*. InitAndModifyConfiguration(*config): Some services need to modify the
//      configuration in order to pass info to higher-level services. This
//      is not common. Internally, bootstrap calls InitAndModifyConfiguration(),
//      which defaults to calling Init().
//  3.  Start: Start triggers the services to begin any work it needs to perform,
//      such as launching threads.
//  4.  Finish: Shut the service down.
//  5.  GetBootstrapDependencies: This function is used to pass along this
//      service's list of dependencies to bootstrap


// In this example, we're creating two singleton services: A and B. We define
// a registration function for each which can be supplied to Init.

#include <iostream>

#include "faodel-common/Common.hh"
#include "faodel-common/Bootstrap.hh"
#include "faodel-common/BootstrapInterface.hh"

using namespace std;
using namespace faodel;


namespace serviceA {
class A
  : public faodel::bootstrap::BootstrapInterface {

public:
  A()  { cout << "A()\n"; }
  ~A() { cout << "~A()\n"; }

  //Bootstrap API
  void Init(const faodel::Configuration &config) override { cout <<"A: Init\n";}
  void Start() override { cout <<"A: Start\n"; }
  void Finish() override { cout <<"A: Finish\n"; }
  void GetBootstrapDependencies(std::string &name,
                       std::vector<std::string> &requires,
                       std::vector<std::string> &optional) const override {
    name="A";
    requires={};
    optional={};
  }
};

//We need to register this component with bootstrap. The easiest way to do this
//is by making a singleton and registering it as well as any dependencies
string bootstrap() {
  static A singleton_a;
  cout <<"Register A\n";
  faodel::bootstrap::RegisterComponent(&singleton_a);
  return "A"; //<--Note: You MUST return the name of the last component
}

} // namespace serviceA


namespace serviceB {
class B
  : public faodel::bootstrap::BootstrapInterface {

public:
  B()  { cout << "B()\n"; }
  ~B() { cout << "~B()\n"; }

  //Bootstrap API
  void Init(const faodel::Configuration &config) override { cout <<"B: Init\n";}
  void Start() override { cout <<"B: Start\n"; }
  void Finish() override { cout <<"B: Finish\n"; }
  void GetBootstrapDependencies(std::string &name,
                                std::vector<std::string> &requires,
                                std::vector<std::string> &optional) const override {
    name="B";
    requires={"A"};
    optional={};
  }
};
//We need to register this component with bootstrap. The easiest way to do this
//is by making a singleton and registering it as well as any dependencies
string bootstrap() {

  //Register everything that A requires
  serviceA::bootstrap();

  //Register our singleton
  static B singleton_b;
  cout <<"Register B\n";
  faodel::bootstrap::RegisterComponent(&singleton_b);
  return "B"; //<--Note: You MUST return the name of the last component
}
} // namespace serviceB



int main(int argc, char **argv) {

  Configuration config;
  if((argc>1) && (string(argv[1]) == "-d"))
    config.Set("bootstrap.debug", "true");


  cout <<"====================================\n";
  cout <<"Launching with just service A\n";
  cout <<"Calling Init\n";    bootstrap::Init(config, serviceA::bootstrap );
  cout <<"Calling Start\n";   bootstrap::Start();
  cout <<"Calling Finish\n";  bootstrap::Finish();

  cout <<"====================================\n";
  cout <<"Launching with both A and B services\n";
  cout <<"Calling Init\n";    bootstrap::Init(config, serviceB::bootstrap );
  cout <<"Calling Start\n";   bootstrap::Start();
  cout <<"Calling Finish\n";  bootstrap::Finish();

  //bootstrap::Init(config, mpisyncstart::bootstrap);

  return 0;
}
