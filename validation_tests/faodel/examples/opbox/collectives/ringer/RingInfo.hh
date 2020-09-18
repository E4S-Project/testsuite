// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#ifndef RINGINFO_HH
#define RINGINFO_HH

#include <vector>
#include <sstream>

#include "faodel-common/Common.hh"

//Forward refs
namespace opbox { class OpArgs; }

class RingInfo {

public:
  RingInfo() {}
  ~RingInfo() {}

  void AddNewNode(faodel::nodeid_t nid) { nodes.push_back(nid); }
  int GetNumValues() { return values.size(); }

  faodel::nodeid_t GetNextNode(){
    if(values.size()>=nodes.size()){
      return faodel::NODE_UNSPECIFIED;
    }
    return nodes[values.size()];
  }
  faodel::nodeid_t AddValueAndGetNextNode(std::string s) {
    values.push_back(s);
    return GetNextNode();
  }

  std::string GetResults() {
    std::stringstream ss;
    for(int i=0; i<nodes.size(); i++){
      ss<<"["<<i<<"]\tNode: "<<nodes[i].GetHex()
        <<"\tContributed Data:\t'"<< ((i>=values.size()) ? "" : values[i])
        <<"'"<<std::endl;
    }
    return ss.str();
  }

  //Serialization hook
  template <typename Archive>
  void serialize(Archive &ar, const unsigned int version){
    ar & nodes;
    ar & values;
  }

private:
  std::vector<faodel::nodeid_t> nodes;
  std::vector<std::string>       values;


};



#endif // RINGINFO_HH
