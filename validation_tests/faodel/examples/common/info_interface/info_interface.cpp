// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

// info_interface
//
// The info_interface is a simple way to dump debug info for a class. It
// is meant for hierarchical class structures, and has a depth option
// to allow you to control how deep a dump goes.

#include <iostream>
#include <string>

#include "faodel-common/Common.hh"

using namespace std;


class B : public faodel::InfoInterface {
public:
  B(string name) : my_name(name) {}

  void sstr(std::stringstream &ss, int depth=0, int indent=0) const {
    ss << string(indent,' ') + "[B:"+my_name+"]\n";
  }
private:
  int my_val;
  string my_name;
};



class A : public faodel::InfoInterface {
public:
  A(string name) : my_name(name), b1(name+"-B1"), b2(name+"-B2") {}

  void sstr(std::stringstream &ss, int depth=0, int indent=0) const {
    ss << string(indent,' ') + "[A:"+my_name+"]\n";
    if(depth<=0) return;
    b1.sstr(ss, depth-1, indent+2);
    b2.sstr(ss, depth-1, indent+2);
  }
private:
  string my_name;
  B b1;
  B b2;
};


int main(int argc, char* argv[]) {

  B b1("lone guy");
  cout << b1.str();  //Only show top info
  cout << b1.str(1); //Dig one layer deeper, which is same

  cout <<"Now for some hierarchy:\n";

  A a1("the a guy");
  cout << a1.str();   //Only show top info
  cout << a1.str(1);  //Dig one layer deeper

  return 0;
}
