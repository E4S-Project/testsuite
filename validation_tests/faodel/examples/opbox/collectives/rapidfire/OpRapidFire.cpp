// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <assert.h>

#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "faodel-common/Debug.hh"

#include "OpRapidFire.hh"

using namespace std;

//Static names/ids for this op
const unsigned int OpRapidFire::op_id = const_hash("OpRapidFire");
const string OpRapidFire::op_name = "OpRapidFire";


OpRapidFire::OpRapidFire(faodel::nodeid_t dst_node, int num_messages, bool dst_reuse_mailbox)
  : state(State::snd_OneAtATime), dst_node(dst_node),
    messages_sent(0), num_messages(num_messages),
    ldo_msg(), dst_reuse_mailbox(dst_reuse_mailbox), Op(true) {

  //Lookup the first member in the ring
  assert(dst_node!=faodel::NODE_UNSPECIFIED && "Destination node not specified?");

  //We'll need to convert this to a peer_ptr_t for send command
  peer = opbox::net::ConvertNodeIDToPeer(dst_node);

  //Work picks up again in origin's state machine in snd_OneAtATime

}

OpRapidFire::OpRapidFire(op_create_as_target_t t)
  : state(State::start), ldo_msg(), Op(t) {
  //No work to do - done in target's state machine
}

OpRapidFire::~OpRapidFire(){
}

future<int> OpRapidFire::GetFuture(){
  return rf_promise.get_future();
}

void OpRapidFire::createOutgoingMessage(faodel::nodeid_t receiver_node,
                                        const mailbox_t &sndr_mailbox,
                                        const mailbox_t &rcvr_mailbox,
                                        int message_id,
                                        int num_messages,
                                        bool reuse_mailbox){

  //Create an ldo for the message and added info
  ldo_msg = opbox::net::NewMessage( sizeof(rapidfire_message_t) );
  rapidfire_message_t *msg = ldo_msg.GetDataPtr<rapidfire_message_t *>();
  msg->hdr.src           = GetMyID();
  msg->hdr.dst           = receiver_node;
  msg->hdr.src_mailbox   = sndr_mailbox;
  msg->hdr.dst_mailbox   = rcvr_mailbox;
  msg->hdr.op_id         = OpRapidFire::op_id;
  msg->hdr.user_flags    = 0; //Set below
  msg->hdr.body_len      = msg->getBodyLen();
  msg->msg_id            = message_id;
  msg->num_msgs          = num_messages;
  msg->ReuseMailbox(reuse_mailbox);
}




WaitingType OpRapidFire::UpdateOrigin(OpArgs *args) {

  switch(state){
  case State::start:
    //Message is already packed and peer is set. Fire and forget.
    KTODO("The Origin constructor sets the initial state to 'snd_OneAtATime'.  Can the 'start' state be eliminated?");
    return WaitingType::waiting_on_cq;


  //One-At-A-Time: This version sends one message at a time, allocating a new
  //ldo for the outgoing message each time.
  case State::snd_OneAtATime:
    {
      opbox::mailbox_t target_mailbox;

      //First time: No message to send, no mailbox to handle
      //All others: We parse the incoming message
      if(messages_sent==0){
        target_mailbox = MAILBOX_UNSPECIFIED;
      } else {

        //Parse incoming and check response
        auto msg = args->ExpectMessageOrDie<rapidfire_message_t *>();
        target_mailbox = msg->hdr.src_mailbox;
        assert(msg->msg_id == (messages_sent-1));
        assert(msg->num_msgs == num_messages);
        cout <<"Origin got reply "<<msg->msg_id<<endl;
      }

      //Send the next message or be done
      if(messages_sent < num_messages){
        //Create and fill a new LDO, just for this message
        createOutgoingMessage(dst_node,
                              GetAssignedMailbox(), target_mailbox,
                              messages_sent, num_messages,
                              dst_reuse_mailbox);

        opbox::net::SendMsg(peer, std::move(ldo_msg));
        messages_sent++;
        return WaitingType::waiting_on_cq;

      } else {
        //No more messages to send. Done.
        state=State::done;
        rf_promise.set_value(messages_sent);
        return WaitingType::done_and_destroy;
      }

    }


  case State::done:
    return WaitingType::done_and_destroy;
  }
  //Shouldn't be here
  KFAIL();
  return WaitingType::error;
}

WaitingType OpRapidFire::UpdateTarget(OpArgs *args) {


  switch(state){

  case State::start:
    {
      auto msg = args->ExpectMessageOrDie<rapidfire_message_t *>(&peer);
      bool reuse_mailbox = msg->ReuseMailbox();
      createOutgoingMessage(msg->hdr.src,
                            (reuse_mailbox) ? GetAssignedMailbox() : MAILBOX_UNSPECIFIED,
                            msg->hdr.src_mailbox,
                            msg->msg_id,
                            msg->num_msgs,
                            reuse_mailbox);

      cout <<"Target got id "<<msg->msg_id<<" my mailboxes: mine="<<GetAssignedMailbox()<<" origin="<<msg->hdr.src_mailbox<< " ReuseMbox=" <<reuse_mailbox<<endl;

      opbox::net::SendMsg(peer, std::move(ldo_msg));

      if((!reuse_mailbox) || (msg->msg_id+1 == msg->num_msgs)){
        cout <<"Target shutting down\n";
        state=State::done;
        return WaitingType::done_and_destroy;
      } else {
        return WaitingType::waiting_on_cq;
      }
    }


  case State::done:
    return WaitingType::done_and_destroy;
  }
  KHALT("Missing state");
  return WaitingType::done_and_destroy;
}

string OpRapidFire::GetStateName() const {
  switch(state){
  case State::start:                 return "Start";
  case State::snd_OneAtATime:        return "Sender-One-At-A-Time";
  case State::done:                  return "Done";
  }
  KFAIL();
  return "Unknown";
}
