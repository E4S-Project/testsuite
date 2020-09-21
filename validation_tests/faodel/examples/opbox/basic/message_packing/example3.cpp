// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <iostream>
#include <string.h>

#include "opbox/OpBox.hh"
#include "opbox/common/MessageHelpers.hh"

using namespace std;


void example3_ldo_string_request_reply(){

  //Client/server ops often send a request that is answered with a reply.
  //Since it's a pain to fill in all of those fields all the time, we
  //added some helpers that can build a reply message off a request.

  //-- Client Side ------------------------------------------------------------
  //The client needs to generate a request
  lunasa::DataObject ldo_request;

  //Allocate a message and fill in the standard info
  AllocateStringRequestMessage(ldo_request,
                          opbox::GetMyID(), //Usually the destination
                          1001, //Usually this op's mailbox
                          2112, //Usually this op's opid
                          0x01,
                          "This is a request");

  //-- Server Side ------------------------------------------------------------

  //We can look at the message by casting the data portion of the ldo
  auto msg_request = ldo_request.GetDataPtr<message_t *>();


  //Extract the message and then acknowledge all-caps day
  string s = UnpackStringMessage(msg_request);
  transform(s.begin(), s.end(), s.begin(), ::toupper);

  //Allocate a reply and use the incoming message to fill in fields
  lunasa::DataObject ldo_reply;
  AllocateStringReplyMessage(ldo_reply,
                             msg_request,
                             0x02,
                             "Reply message for "+s);

  //-- Client Side ------------------------------------------------------------

  //Cast and extract the message
  auto msg_reply = ldo_reply.GetDataPtr<message_t *>();
  string s2 = UnpackStringMessage(msg_reply);

  cout <<"Reply message was '"<<s2<<"'\n";

}

