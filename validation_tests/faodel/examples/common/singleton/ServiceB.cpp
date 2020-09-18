// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include "ServiceB.hh"

#include "faodel-common/Common.hh"

namespace service_b {


//This internal implementation of ServiceB holds the bootstrap code for the
//service. This impl allows bootstrap calls to be hidden from the user.

class ServiceB_Impl
  : public faodel::bootstrap::BootstrapInterface {

public:
  ServiceB_Impl();
  ~ServiceB_Impl();

  //Bootstrap API
  void Init(const faodel::Configuration &config);
  void Start();
  void Finish();
  void GetBootstrapDependencies(std::string &name,
                       std::vector<std::string> &requires,
                       std::vector<std::string> &optional) const;

  //Actual command
  void doOp(std::string command);

private:
  bool started;
  int op_count=0;

};

//Initialize the static variable in the external class so the bootstrap
//registration code gets called.
ServiceB_Impl ServiceB::impl;



using namespace std;

ServiceB_Impl::ServiceB_Impl()
  : started(false) {

  faodel::bootstrap::RegisterComponent(this);

}
ServiceB_Impl::~ServiceB_Impl() {
  if(started) cout <<"ServiceB_Impl started but not stopped in shutdown\n";
}

void ServiceB_Impl::Init(const faodel::Configuration &config) {
  cout <<"ServiceB_Impl Init()\n";
}
void ServiceB_Impl::Start() {
  started=true;
  cout <<"ServiceB_Impl started\n";
}
void ServiceB_Impl::Finish() {
  if(!started) cout <<"ServiceB_Impl Finish called before Started?\n";
  cout <<"ServiceB_Impl Finish\n";
  started=false;
}
void ServiceB_Impl::GetBootstrapDependencies(
                       string &name,
                       vector<string> &requires,
                       vector<string> &optional) const {
  name = "service_b";
  requires = {"service_a"};
  optional = {};
}
void ServiceB_Impl::doOp(string cmd) {
  if(!started) cout <<"ServiceB_Impl doOp called when service not started\n";
  cout <<"ServiceB_Impl doOp #"<<op_count++<<" is "<<cmd<<endl;
}




//Pass the exterior calls down to impl
ServiceB::ServiceB() {}
ServiceB::~ServiceB() {}
void ServiceB::doOp(string cmd) { return impl.doOp(cmd); }

} // namespace ServiceB
