// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <assert.h>
#include <string.h> //for memcpy

#include "opbox/ops/OpHelpers.hh"

#include "OpRdmaPing.hh"


using namespace std;

const unsigned int OpRdmaPing::op_id = const_hash("OpRdmaPing");
const string OpRdmaPing::op_name = "OpRdmaPing";



OpRdmaPing::OpRdmaPing(opbox::net::peer_ptr_t dst, lunasa::DataObject msg)
  : state(State::start), ldo_msg(), Op(true) {
  peer = dst;
  ping_ldo = msg;
  createOutgoingMessage(opbox::net::ConvertPeerToNodeID(dst),
                        GetAssignedMailbox(),
                        0,
                        ping_ldo);
  //Work picks up again in origin's state machine
}

OpRdmaPing::OpRdmaPing(op_create_as_target_t t)
  : state(State::start), ldo_msg(), Op(t) {
  //No work to do - done in target's state machine
}

OpRdmaPing::~OpRdmaPing(){
}

future<std::string> OpRdmaPing::GetFuture(){
  return ping_promise.get_future();
}

/*
 * Create a message to initiate the communication with the target.
 * This message starts with a message_t that carries basic
 * information about the origin (us) and the target (them).  Following
 * the message_t is the body of the message.  The body of this
 * message is a NetBufferRemote that describes the RDMA window of the
 * ping message.
 */
void OpRdmaPing::createOutgoingMessage(faodel::nodeid_t   dst,
                                       const mailbox_t    &src_mailbox,
                                       const mailbox_t    &dst_mailbox,
                                       lunasa::DataObject  ping_ldo){

  ldo_msg = opbox::net::NewMessage(sizeof(message_t)+sizeof(struct opbox::net::NetBufferRemote));
  message_t *msg = ldo_msg.GetDataPtr<message_t *>();
  msg->src           = opbox::net::GetMyID();
  msg->dst           = dst;
  msg->src_mailbox   = src_mailbox;
  msg->dst_mailbox   = dst_mailbox;
  msg->op_id         = OpRdmaPing::op_id;
  msg->body_len      = sizeof(struct opbox::net::NetBufferRemote);

  opbox::net::NetBufferLocal  *nbl = nullptr;
  opbox::net::NetBufferRemote  nbr;
  opbox::net::GetRdmaPtr(&ping_ldo, &nbl, &nbr);

  memcpy(&msg->body[0], &nbr, sizeof(opbox::net::NetBufferRemote));
}

/*
 * Create a message to terminate communication with the origin.  This
 * message is just a message_t that tells the origin (them) which
 * operation is complete.
 */
void OpRdmaPing::createAckMessage(faodel::nodeid_t   dst,
                                  const mailbox_t    &src_mailbox,
                                  const mailbox_t    &dst_mailbox){

  ldo_msg = opbox::net::NewMessage(sizeof(message_t));
  message_t *msg = ldo_msg.GetDataPtr<message_t *>();
  msg->src           = opbox::net::GetMyID();
  msg->dst           = dst;
  msg->src_mailbox   = src_mailbox;
  msg->dst_mailbox   = dst_mailbox;
  msg->op_id         = OpRdmaPing::op_id;
  msg->body_len      = 0;
}

/*
 * This is the origin state machine.
 */
WaitingType OpRdmaPing::UpdateOrigin(OpArgs *args) {
  string user_data;

  switch(state){
  case State::start:
    // send a message to the target process to begin the ping.
    opbox::net::SendMsg(peer, std::move(ldo_msg));

    state=State::snd_wait_for_ack;
    return WaitingType::waiting_on_cq;

  case State::snd_wait_for_ack:
    // the ACK message has arrived
    args->ExpectMessageOrDie<message_t *>(); //TODO ack/nack check

    // extract the transformed ping message from the LDO and create a string from it
    user_data = string(ping_ldo.GetDataPtr<char *>(), ping_ldo.GetDataSize());

    // set the the value of the promise which will wake up the main
    // thread which is waiting on the associated future
    ping_promise.set_value(user_data);

    state=State::done;
    return WaitingType::done_and_destroy;

  case State::done:
    return WaitingType::done_and_destroy;
  }
  //Shouldn't be here
  KFAIL();
  return WaitingType::error;
}

/*
 * This is the target state machine.
 */
WaitingType OpRdmaPing::UpdateTarget(OpArgs *args) {

  switch(state){

  case State::start:
    {
      auto incoming_msg = args->ExpectMessageOrDie<message_t *>(&peer);

      // save a copy of the NBR for later use
      memcpy(&nbr, &incoming_msg->body[0], sizeof(opbox::net::NetBufferRemote));
      // this is the initiator buffer for the get and the put
      shout_ldo = lunasa::DataObject(0, nbr.GetLength(), lunasa::DataObject::AllocatorType::eager);

      // we create the ACK message now be cause we need the sender's
      // node ID and the mailbox ID from the incoming message.
      // instead of saving the message, just use what we need.
      createAckMessage(incoming_msg->src,
                       0, //Not expecting a reply
                       incoming_msg->src_mailbox);

      // get the ping message from the origin process.
      // AllEventsCallback() is a convenience class that will redirect
      // all events generated by the get to this operation's Update()
      // method.
      opbox::net::Get(peer, &nbr, shout_ldo, AllEventsCallback(this));

      state=State::get_wait_complete;
      return WaitingType::waiting_on_cq;
    }

  case State::get_wait_complete:
    // the get is complete, so transform the ping message in-place.
    //for(int i=0; i<shout_ldo.dataSize(); i++) {
    //  (shout_ldo.dataPtr<char *>())[i] = toupper(shout_ldo.dataPtr<char *>())[i]);
    //}
    {
      auto *data = shout_ldo.GetDataPtr<char *>();
      for(int i=0; i<shout_ldo.GetDataSize(); i++)
        data[i] = toupper(data[i]);
    }

    // put the transformed ping message back to the origin process.
    // AllEventsCallback() is a convenience class that will redirect
    // all events generated by the put to this operation's Update()
    // method.
    opbox::net::Put(peer, shout_ldo, &nbr, AllEventsCallback(this));

    state=State::put_wait_complete;
    return WaitingType::waiting_on_cq;

  case State::put_wait_complete:
    // the put is complete, so send the ACK to the origin process.
    opbox::net::SendMsg(peer, std::move(ldo_msg));

    state=State::done;
    return WaitingType::done_and_destroy;

  case State::done:
    return WaitingType::done_and_destroy;
  }
  KHALT("Missing state");
  return WaitingType::done_and_destroy;
}

string OpRdmaPing::GetStateName() const {
  switch(state){
  case State::start:             return "Start";
  case State::snd_wait_for_ack:  return "Sender-WaitForAck";
  case State::get_wait_complete: return "GetWaitComplete";
  case State::put_wait_complete: return "PutWaitComplete";
  case State::done:              return "Done";
  }
  KFAIL();
}
