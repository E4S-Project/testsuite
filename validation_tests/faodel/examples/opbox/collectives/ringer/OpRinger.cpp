// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <assert.h>

#include <boost/serialization/vector.hpp>

#include "opbox/common/MessageHelpers.hh"

#include "OpRinger.hh"

using namespace std;

//Static names/ids for this op
const unsigned int OpRinger::op_id = const_hash("OpRinger");
const string OpRinger::op_name = "OpRinger";


OpRinger::OpRinger(RingInfo ring_info)
  : state(State::start), ldo_msg(), Op(true) {

  //Lookup the first member in the ring
  faodel::nodeid_t dst_node = ring_info.GetNextNode();
  assert(dst_node!=faodel::NODE_UNSPECIFIED && "RingInfo didn't have any members?");

  //We'll need to convert this to a peer_ptr_t for send command
  peer = opbox::net::ConvertNodeIDToPeer(dst_node);

  //We can use the standard boost request function to pack this message
  opbox::AllocateBoostRequestMessage<RingInfo>(ldo_msg,
                                        dst_node, GetAssignedMailbox(),
                                        op_id, 0,
                                        ring_info);


  //Work picks up again in origin's state machine
}

OpRinger::OpRinger(op_create_as_target_t t)
  : state(State::start), ldo_msg(), Op(t) {
  //No work to do - done in target's state machine
}

OpRinger::~OpRinger(){
}

future<RingInfo> OpRinger::GetFuture(){
  return ring_promise.get_future();
}


WaitingType OpRinger::UpdateOrigin(OpArgs *args) {

  switch(state){
  case State::start:
    //Message is already packed and peer is set. Fire and forget.
    opbox::net::SendMsg(peer, std::move(ldo_msg));
    state=State::snd_wait_for_reply;
    return WaitingType::waiting_on_cq;

  case State::snd_wait_for_reply:
    {
      //We expect to have received a message with RingInfo in it. Unpack
      //and send it back to the user via a promise
      auto msg = args->ExpectMessageOrDie<message_t *>();
      auto ring_info = UnpackBoostMessage<RingInfo>(msg);
      ring_promise.set_value(ring_info);

      state=State::done;
      return WaitingType::done_and_destroy;
    }
  case State::done:
    return WaitingType::done_and_destroy;
  }
  //Shouldn't be here
  KFAIL();
  return WaitingType::error;
}

WaitingType OpRinger::UpdateTarget(OpArgs *args) {

  std::stringstream ss;

  switch(state){

  case State::start:
    {
      cout<<"Target from "<<GetMyID().GetHex() <<endl;

      //New message should have a RingInfo we can use for locating next node
      auto msg = args->ExpectMessageOrDie<message_t *>();
      auto ring_info = UnpackBoostMessage<RingInfo>(msg);

      //Create a message for this node and add it to the ring
      int spot = ring_info.GetNumValues();
      ss << "This is data from spot "<<spot<<" node "<<GetMyID().GetHex();

      faodel::nodeid_t next_node = ring_info.AddValueAndGetNextNode(ss.str());
      mailbox_t dst_mailbox = 0; //Unexpected message

      //See if we were the last node on the list. If yes, send to origin
      if(next_node==faodel::NODE_UNSPECIFIED){
        next_node   = msg->src;
        dst_mailbox = msg->src_mailbox;
      }

      //Convert the next node id to a peer ptr net can use
      peer = opbox::net::ConvertNodeIDToPeer(next_node);

      //We're going to forward a request on to the next node, using origin's info
      AllocateBoostMessage<RingInfo>(ldo_msg,
                                     msg->src, next_node,
                                     msg->src_mailbox, dst_mailbox,
                                     op_id, 0,
                                     ring_info);

      state=State::done;

      //Send the message
      opbox::net::SendMsg(peer, std::move(ldo_msg));

      //This node is done

      return WaitingType::waiting_on_cq;
    }
  case State::done:
    cout <<"Done waiting: "<<GetMyID().GetHex()<<endl;
    return WaitingType::done_and_destroy;
  }
  KHALT("Missing state");
  return WaitingType::done_and_destroy;
}

string OpRinger::GetStateName() const {
  switch(state){
  case State::start:              return "Start";
  case State::snd_wait_for_reply: return "Sender-WaitForReply";
  case State::done:               return "Done";
  }
  KFAIL();
  return "Unknown";
}
