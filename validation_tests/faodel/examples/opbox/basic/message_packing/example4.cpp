// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <iostream>
#include <sstream>
#include <string.h>
#include <boost/serialization/vector.hpp>

#include "opbox/OpBox.hh"
#include "opbox/common/MessageHelpers.hh"

using namespace std;

// Boost's serialization library provides an easy way for us to ser/des a
// complex class. In addition to being hierarchical, it also supports
// STL containers (provided you include the corresponding header file.
// eg. boost/serialization/vector.hpp). All you need to do to make a
// class boost serializable is (1) make sure there's a default constructor
// and (2) provide a "serialize" template that specifies which variables
// are archived.
//
// - Caveat 1: Boost serialization can be expensive. In addition to
//             streaming its data as it moves along, the OpBox code
//             has to copy the packed string of data into the outgoing
//             message.
// - Caveat 2: You need a serialize template on every class that gets
//             pulled into an archive.
// - Caveat 3: Each constructor must have a default constructor in order
//             for the deserialization to work.
//
// Similar to the string messages, we have request/reply versions of
// the allocator that make it easier to reply to a request.

// In this example, we have a FancyPants class that has a vector of
// Pockets.

class Pocket {
public:
  Pocket() {}
  Pocket(bool is_left, bool is_front)
    : is_left(is_left), is_front(is_front) {}
  bool is_left;
  bool is_front;

  string str(){
    stringstream ss;
    ss << ((is_front)?"front-":"back-")<<((is_left)?"left":"right")<< " pocket";
    return ss.str();
  }

  //Serialization hook
  template <typename Archive>
  void serialize(Archive &ar, const unsigned int version){
    ar & is_left;
    ar & is_front;
  }
};

class FancyPants {
public:
  FancyPants() {}
  FancyPants(string name, string color, int num_pockets)
    : name(name), color(color) {
    for(int i=0; i<num_pockets; i++)
      pockets.push_back( Pocket((i&0x01), (i&0x02)) );
  }

  string name;
  string color;
  vector<Pocket> pockets;

  string str(){
    stringstream ss;
    ss <<color<<"-"<<name<<endl;
    for(auto p : pockets)
      ss << "   "<<p.str()<<endl;
    return ss.str();
  }

  //Serialization hook
  template <typename Archive>
  void serialize(Archive &ar, const unsigned int version){
    ar & name;
    ar & color;
    ar & pockets;
  }
};


void example4_boost_messages(){

  //Create some fancy pants for us to pack into a message
  FancyPants fancy_pants("britches", "red", 4);

  lunasa::DataObject ldo;

  //Use the templated allocator to create a message.
  AllocateBoostMessage<FancyPants>( ldo,
                                    opbox::GetMyID(),
                                    opbox::GetMyID(), //Usually dst node
                                    100,              //Usually supplied by op
                                    opbox::MAILBOX_UNSPECIFIED,
                                    2112, 0x01,
                                    fancy_pants);     //Our class

  //Get the ldo's data pointer and cat to a message
  auto msg = ldo.GetDataPtr<message_t *>();

  cout <<"LDO Size is: "      << ldo.GetDataSize()<<endl
       <<"Packed boost size: "<< msg->body_len<<endl;

  //Unpack the message into a new object
  auto fp2 = UnpackBoostMessage<FancyPants>(msg);

  //Dump out the original and unpacked classes
  cout <<"Original class:\n"<<fancy_pants.str() << endl
       <<"Unpacked class:\n"<<fp2.str()<<endl;
}


