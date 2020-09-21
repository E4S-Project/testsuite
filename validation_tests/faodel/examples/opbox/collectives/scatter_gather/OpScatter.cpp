// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <assert.h>

#include <cstring>

#include "opbox/ops/OpHelpers.hh"

#include "SimpleDataStore.hh"
#include "OpScatter.hh"

using namespace std;

const unsigned int OpScatter::op_id = const_hash("OpScatter");
const string OpScatter::op_name = "OpScatter";


OpScatter::OpScatter(opbox::net::peer_ptr_t *peer_list, int peer_count, lunasa::DataObject scatter_src_ldo)
        : state(State::start), scatter_count(0), Op(true) {
  this->peer_list = peer_list;
  this->peer_count = peer_count;
  this->scatter_ldo = scatter_src_ldo;
  //Work picks up again in origin's state machine
}

OpScatter::OpScatter(op_create_as_target_t t)
        : state(State::start), scatter_count(0), Op(t) {
  //No work to do - done in target's state machine
}

OpScatter::~OpScatter() {
}

future<int> OpScatter::GetFuture() {
  return scatter_promise.get_future();
}

/*
 * Create a message to initiate the communication with the target.
 * This message starts with a Message that carries basic
 * information about the origin (us) and the target (them).  Following
 * the Message is the body of the message.  The body of this
 * message is the key used to store the scattered data plus a
 * NetBufferRemote that describes the RDMA window of the
 * scattered data.
 */
lunasa::DataObject OpScatter::createOutgoingMessage(faodel::nodeid_t dst,
                                                    const mailbox_t &src_mailbox,
                                                    const mailbox_t &dst_mailbox,
                                                    const uint32_t block_length,
                                                    const int target_rank) {
  scatter_key key;
  int key_size = sizeof(scatter_key);

  lunasa::DataObject ldo_msg = opbox::net::NewMessage(
          sizeof(message_t) + key_size + sizeof(struct opbox::net::NetBufferRemote));

  message_t *msg = ldo_msg.GetDataPtr<message_t *>();
  msg->src           = opbox::net::GetMyID();
  msg->dst           = dst;
  msg->src_mailbox   = src_mailbox;
  msg->dst_mailbox   = dst_mailbox;
  msg->op_id         = OpScatter::op_id;
  msg->body_len      = key_size + sizeof(struct opbox::net::NetBufferRemote);

  opbox::net::NetBufferLocal *nbl = nullptr;
  opbox::net::NetBufferRemote nbr;
  // We don't want the target to reference the entire LDO, so we
  // specify an offset and length that creates a widow in the LDO.
  // Since a complete LDO includes the header, the offset must include
  // the size of the header.
  int rc = opbox::net::GetRdmaPtr(&scatter_ldo, scatter_ldo.GetHeaderSize() + (block_length*target_rank), block_length,
                                  &nbl, &nbr);
  assert(rc == 0 && "opbox::net::GetRdmaPtr()");

  memset(key, 0, key_size);
  // create a key that is unique for each target
  sprintf(key, "RANK%0u", target_rank);
  memcpy(&msg->body[0], key, key_size);
  memcpy(&msg->body[key_size], &nbr, sizeof(opbox::net::NetBufferRemote));

  return ldo_msg;
}

/*
 * Create a message to terminate communication with the origin.  This
 * message is just a message_t that tells the origin (them) which
 * operation is complete.
 */
lunasa::DataObject OpScatter::createAckMessage(faodel::nodeid_t dst,
                                               const mailbox_t &src_mailbox,
                                               const mailbox_t &dst_mailbox) {
  lunasa::DataObject ldo_msg = opbox::net::NewMessage(sizeof(message_t));
  auto *msg = ldo_msg.GetDataPtr<message_t *>();
  msg->src           = opbox::net::GetMyID();
  msg->dst           = dst;
  msg->src_mailbox   = src_mailbox;
  msg->dst_mailbox   = dst_mailbox;
  msg->op_id         = OpScatter::op_id;
  msg->body_len      = 0;

  return ldo_msg;
}

/*
 * This is the origin state machine.
 */
WaitingType OpScatter::UpdateOrigin(OpArgs *args) {
  uint32_t block_size;
  lunasa::DataObject ldo_msg;
  string user_data;

  switch(state) {
    case State::start:
      // evenly divide the scattered data amongst the targets
      block_size = scatter_ldo.GetDataSize()/peer_count;
      // create and send the init message to each target
      for(int i = 0; i<peer_count; i++) {
        ldo_msg = createOutgoingMessage(opbox::net::ConvertPeerToNodeID(peer_list[i]),
                                        GetAssignedMailbox(),
                                        MAILBOX_UNSPECIFIED,
                                        block_size,
                                        i);

        // send a message to the target process to begin the scatter
        opbox::net::SendMsg(peer_list[i], std::move(ldo_msg));
      }

      state = State::snd_wait_for_ack;
      return WaitingType::waiting_on_cq;

    case State::snd_wait_for_ack:
      // an ACK message has arrived
      args->VerifyTypeOrDie(UpdateType::incoming_message, op_name);

      // keep track of how many targets have sent an ACK
      scatter_count++;
      // if we don't have all the ACKs, keep waiting
      if(scatter_count<peer_count) {
        return WaitingType::waiting_on_cq;
      }

      // set the the value of the promise which will wake up the main
      // thread which is waiting on the associated future
      scatter_promise.set_value(1);

      state = State::done;
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
WaitingType OpScatter::UpdateTarget(OpArgs *args) {
  int key_size = sizeof(scatter_key);

  switch(state) {

    case State::start: {
      auto incoming_msg = args->ExpectMessageOrDie<message_t *>(&origin);

      // save a copy of the key
      memcpy(key, &incoming_msg->body[0], key_size);
      // save a copy of the NBR for later use
      memcpy(&nbr, &incoming_msg->body[14], sizeof(opbox::net::NetBufferRemote));
      // this is the initiator buffer for the get
      scatter_ldo = lunasa::DataObject(0, nbr.GetLength(), lunasa::DataObject::AllocatorType::eager);

      // we create the ACK message now be cause we need the sender's
      // node ID and the mailbox ID from the incoming message.
      // instead of saving the message, just use what we need.
      ack_msg = createAckMessage(incoming_msg->src,
                                 0, //Not expecting a reply
                                 incoming_msg->src_mailbox);

      // get the origin's data (skipping the header) from the NBR they
      // sent us.
      // AllEventsCallback() is a convenience class that will redirect
      // all events generated by the get to this operation's Update()
      // method.
      opbox::net::Get(origin, &nbr, 0, scatter_ldo, scatter_ldo.GetHeaderSize(), nbr.GetLength(),
                      AllEventsCallback(this));

      state = State::get_wait_complete;
      return WaitingType::waiting_on_cq;
    }
    case State::get_wait_complete:

      examples::SimpleDataStore::Add(std::string(key), scatter_ldo);

      // the get is complete, so send the ACK to the origin process.
      opbox::net::SendMsg(origin, std::move(ack_msg));

      state = State::done;
      return WaitingType::done_and_destroy;

    case State::done:
      return WaitingType::done_and_destroy;
  }
  KHALT("Missing state");
  return WaitingType::done_and_destroy;
}

string OpScatter::GetStateName() const {
  switch(state){
  case State::start:             return "Start";
  case State::snd_wait_for_ack:  return "Sender-WaitForAck";
  case State::get_wait_complete: return "GetWaitComplete";
  case State::done:              return "Done";
  }
  KFAIL();
  return "Unknown";
}
