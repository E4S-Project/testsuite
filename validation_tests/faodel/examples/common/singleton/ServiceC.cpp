// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include "ServiceC.hh"

#include "faodel-common/Common.hh"

namespace service_c {

class ServiceC
  : public faodel::bootstrap::BootstrapInterface {

public:
  ServiceC();
  ~ServiceC();

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

};

namespace {
ServiceC *service_c=nullptr;
}

using namespace std;



ServiceC::ServiceC()
  : started(false) {
}
ServiceC::~ServiceC() {
  if(started) cout <<"ServiceC started but not stopped in shutdown\n";
}

void ServiceC::Init(const faodel::Configuration &config) {
  cout <<"ServiceC Init()\n";
}
void ServiceC::Start() {
  started=true;
  cout <<"ServiceC started\n";
}
void ServiceC::Finish() {
  if(!started) cout <<"ServiceC Finish called before Started?\n";
  cout <<"ServiceC Finish\n";
  started=false;
}
void ServiceC::GetBootstrapDependencies(
                       string &name,
                       vector<string> &requires,
                       vector<string> &optional) const {
  name = "service_c";
  requires = {"service_b"};
  optional = {};
}
void ServiceC::doOp(string cmd) {
  if(!started) cout <<"ServiceC doOp called when service not started\n";
  cout <<"ServiceC doOp  is "<<cmd<<endl;
}




void doOp(string cmd) {
  if(service_c==nullptr) { cout <<"ServiceC doOp called when service not started\n";}
  service_c->doOp(cmd);
}

void RegisterBootstrap() {
  if(service_c!=nullptr) { cout <<"Attempted to double register service c\n";}
  service_c = new ServiceC();

  faodel::bootstrap::RegisterComponent(service_c);
}
void DeregisterBootstrap() {
  if(service_c==nullptr) { cout <<"Attempted to de-register bootstrap that isn't registered\n"; return;}
  delete service_c;
  service_c=nullptr;
  //todo: bootstrap lacks a way to deregister now
}


} // namespace service_c
