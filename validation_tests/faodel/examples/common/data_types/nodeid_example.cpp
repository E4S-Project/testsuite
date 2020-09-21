// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <iostream>

#include "faodel-common/Common.hh"

using namespace std;
using namespace faodel;

void example_nodeid() {

  cout <<"NodeID Examples\n";

  //A nodeid_t is the fundamental way we reference different ranks
  //that are running in the system. The nodeid is a 64b value that
  //any rank in the system can communicate with.

  //The Whookie service is responsible for managing a communication
  //socket for each rank. This socket is a regular tcp/ip socket
  //(though most transports allow you to run on top of the native
  //hpc interconnect). A rank's nodeid_t can be discovered by
  //querying whookie:
  //   whookie::Server::GetMyID();


  //You can manually specify the ip/port for a node. This is mostly
  //for debugging purposes.
  nodeid_t nid("192.168.1.1","1990");


  //The nodeid_t can be converted to multiple formats.
  cout <<"node ip is   "      <<nid.GetIP()             <<endl;
  cout <<"node port is "      <<nid.GetHttpLink()       <<endl;
  cout <<"node http/link is " <<nid.GetHttpLink("link") <<endl;
  cout <<"node html is "      <<nid.GetHtmlLink()       <<endl;
  cout <<"node hex is  "      <<nid.GetHex()            <<endl;

  //There are two special values:
  //  NODE_LOCALHOST   -  signifies the local host
  //  NODE_UNSPECIFIED -  signifies the node has not been specified


  //When you need to ship the ip info around, it's often more
  //convenient to convert it to a hex value. This hex value is
  //just the port followed by the ip. You can pass the hex value
  //into a nodeid ctor and have the right value generated.
  string hex_val = nid.GetHex();
  nodeid_t nid2(hex_val);
  cout <<"Original Hex: "<<hex_val<<" Copied Hex: "<<nid2.GetHex()<<endl;


  //The nodeid_t is a 64b POD. As such you can use it in other PODs
  //that don't need any fancy serialization
  typedef struct {
    uint32_t x;  //4B
    uint32_t y;  //4B
    nodeid_t n;  //8B
  } my_thing_t;  //=16B

  cout <<"Size of my_thing_t is "<<sizeof(my_thing_t)<<endl;

  //There's also a serialize function though, to let you stick this
  //value into more complex serializers like Boost.



}
