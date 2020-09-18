// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 


#include "ServiceA.hh"


namespace service_a {

class ServiceA
  : public faodel::bootstrap::BootstrapInterface {

public:
  ServiceA();
  ~ServiceA();

  //Bootstrap API
  void Init(const faodel::Configuration &config);
  void Start();
  void Finish();
  void GetBootstrapDependencies(std::string &name,
                       std::vector<std::string> &requires,
                       std::vector<std::string> &optional) const;

  void doOp(std::string command);
private:
  bool started;

};


namespace {
ServiceA service_a; //Global, only available in this file
}



using namespace std;

ServiceA::ServiceA()
  : started(false) {

  faodel::bootstrap::RegisterComponent(this);

}
ServiceA::~ServiceA() {
  if(started) cout <<"ServiceA started but not stopped in shutdown\n";
}

void ServiceA::Init(const faodel::Configuration &config) {
  cout <<"ServiceA Init()\n";
}
void ServiceA::Start() {
  started=true;
  cout <<"ServiceA started\n";
}
void ServiceA::Finish() {
  if(!started) cout <<"ServiceA Finish called before Started?\n";
  cout <<"ServiceA Finish\n";
  started=false;
}
void ServiceA::GetBootstrapDependencies(
                       string &name,
                       vector<string> &requires,
                       vector<string> &optional) const {
  name = "service_a";
  requires = {};
  optional = {};
}
void ServiceA::doOp(string cmd) {
  if(!started) cout <<"ServiceA doOp called when service not started\n";
  cout <<"ServiceA doOp for "<<cmd<<endl;
}


//External API
void doOp(string command) {
  service_a.doOp(command);
}

} // namespace service_a
