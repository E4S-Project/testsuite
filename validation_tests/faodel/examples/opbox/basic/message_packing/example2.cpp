// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <iostream>
#include <string.h>

#include "opbox/OpBox.hh"
#include "opbox/common/MessageHelpers.hh"

using namespace std;


void example2_ldo_messages(){

  //Now that we understand how to pack messages, we need to look at how we
  //can build messages that can be handed to network functions. OpBox's
  //network commands expect an outgoing message to be stored in contiguous,
  //nic-accessible memory. We use Lunasa to manage this memory and store
  //an individual message in a Lunasa Data Object (or LDO).
  //
  //OpBox provides a few functions for simplifying the process of allocating
  //an LDO and storing a message in it. For example, if you're just sending
  //a plain message_t, you can use the AllocateStandardMessage() from
  //MessageHelpers.hh to build the message.


  //In order to avoid refcounting, we use a pointer to an LDO instead of
  //the actual LDO.
  lunasa::DataObject ldo;

  //Allocate a message and fill in the standard info
  AllocateStandardMessage(ldo,
                          opbox::GetMyID(),
                          1001, //Usually this op's mailbox
                          2112, //Usually this op's opid
                          0x01);


  //We can look at the message by casting the data portion of the ldo
  auto msg = ldo.GetDataPtr<message_t *>();

  cout <<"LDO data size is: "<<ldo.GetDataSize()<<endl
       <<"Message size is:  "<<sizeof(message_t)<<endl
       <<"LDO RefCount is:  "<<ldo.internal_use_only.GetRefCount()<<endl;

  cout <<"Message is:\n"<<msg->str()<<endl;


  //Normally we'd do an opbox::net::SendMsg(peer, ldo, events) right
  //after we build the message. The ldo will have a refcount of one
  //and be handed owned by SendMsg. When SendMsg finishes sending, it
  //decrements the refcount, which goes to zero and frees the allocation.

}

void example2_ldo_string_messages(){

  //Wouldn't it be nice if we could just lump on one simple string to
  //the end of that message? We could use it to send one piece of
  //variable length info (which a parser could even split into other
  //pieces). AllocateStringMessage does just that.

  lunasa::DataObject ldo;

  //Allocate a message and fill in the info.
  AllocateStringMessage(  ldo,
                          opbox::GetMyID(),
                          opbox::GetMyID(), //Usually the destination
                          1001, //Usually this op's mailbox
                          opbox::MAILBOX_UNSPECIFIED, //Target's mailbox
                          2112, //Usually this op's opid
                          0x01,
                          "This is my special message");

  //We can look at the message by casting the data portion of the ldo
  auto msg = ldo.GetDataPtr<message_t *>();

  //Use UnpackStringMessage() to extract the daya
  string s = UnpackStringMessage(msg);
  cout <<"Received message '"<<s<<"'\n";

}
