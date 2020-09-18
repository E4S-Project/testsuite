// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <assert.h>
#include "faodel-common/Debug.hh"

#include "OpMySimplePing.hh"

using namespace std;

const unsigned int OpMySimplePing::op_id = const_hash("OpMySimplePing");
const string OpMySimplePing::op_name = "OpMySimplePing";



OpMySimplePing::OpMySimplePing(opbox::net::peer_ptr_t dst, std::string ping_message)
  : state(State::start),
    ldo_msg(),
    Op(true) { //Note: true here signifies that we create the mailbox now

  peer = dst;
  createOutgoingMessage(opbox::net::ConvertPeerToNodeID(dst),
                        GetAssignedMailbox(),
                        MAILBOX_UNSPECIFIED,
                        ping_message);
  //Work picks up again in origin's state machine
}

OpMySimplePing::OpMySimplePing(op_create_as_target_t t)
  : state(State::start), ldo_msg(), Op(t) {
  //No work to do - done in target's state machine
}

OpMySimplePing::~OpMySimplePing(){
}

future<std::string> OpMySimplePing::GetFuture(){
  return ping_promise.get_future();
}

void OpMySimplePing::createOutgoingMessage(faodel::nodeid_t dst,
                                   const mailbox_t &src_mailbox,
                                   const mailbox_t &dst_mailbox,
                                   const string &ping_message){

  ldo_msg = opbox::net::NewMessage( sizeof(message_t)+ping_message.size());
  message_t *msg = ldo_msg.GetDataPtr<message_t *>();
  msg->src           = opbox::net::GetMyID();
  msg->dst           = dst;
  msg->src_mailbox   = src_mailbox;
  msg->dst_mailbox   = dst_mailbox;
  msg->op_id         = OpMySimplePing::op_id;
  msg->body_len      = ping_message.size();
  ping_message.copy(&msg->body[0], ping_message.size());
}

WaitingType OpMySimplePing::UpdateOrigin(OpArgs *args) {

  switch(state){
  case State::start:
    opbox::net::SendMsg(peer, std::move(ldo_msg));
    state=State::snd_wait_for_reply;
    return WaitingType::waiting_on_cq;

  case State::snd_wait_for_reply:
    {
      auto incoming_msg = args->ExpectMessageOrDie<message_t *>();
      auto user_data = string(incoming_msg->body, incoming_msg->body_len);
      ping_promise.set_value(user_data);
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

WaitingType OpMySimplePing::UpdateTarget(OpArgs *args) {

  switch(state){

  case State::start:
    {
      auto incoming_msg =  args->ExpectMessageOrDie<message_t *>(&peer);
      string shout;
      shout.reserve(incoming_msg->body_len);
      for(int i=0; i<incoming_msg->body_len; i++) {
        shout.push_back(toupper(incoming_msg->body[i]));
      }

      createOutgoingMessage(incoming_msg->src,
                            MAILBOX_UNSPECIFIED, //Not expecting a reply
                            incoming_msg->src_mailbox,
                            shout);

      opbox::net::SendMsg(peer, std::move(ldo_msg));
      state=State::done;
      return WaitingType::done_and_destroy;
    }

  case State::done:
    return WaitingType::done_and_destroy;
  }
  KHALT("Missing state");
  return WaitingType::done_and_destroy;
}

string OpMySimplePing::GetStateName() const {
  switch(state){
  case State::start:              return "Start";
  case State::snd_wait_for_reply: return "Sender-WaitForReply";
  case State::done:               return "Done";
  }
  KFAIL();
}
