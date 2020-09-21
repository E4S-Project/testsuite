// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <assert.h>

#include <cstring>

#include "opbox/ops/OpHelpers.hh"

#include "SimpleDataStore.hh"
#include "OpGather.hh"

using namespace std;

const unsigned int OpGather::op_id = const_hash("OpGather");
const string OpGather::op_name = "OpGather";


OpGather::OpGather(opbox::net::peer_ptr_t *peer_list, int peer_count, lunasa::DataObject gather_dst_ldo)
        : state(State::start), gather_count(0), Op(true) {

  this->peer_list = peer_list;
  this->peer_count = peer_count;
  this->gather_ldo = gather_dst_ldo;
  //Work picks up again in origin's state machine
}

OpGather::OpGather(op_create_as_target_t t)
        : state(State::start), gather_count(0), Op(t) {
  //No work to do - done in target's state machine
}

OpGather::~OpGather() {
}

future<int> OpGather::GetFuture() {
  return gather_promise.get_future();
}

/*
 * Create a message to initiate the communication with the target.
 * This message starts with a message_t that carries basic
 * information about the origin (us) and the target (them).  Following
 * the message_t is the body of the message.  The body of this
 * message is the key used to find the gathered data plus a
 * NetBufferRemote that describes the RDMA window of the
 * gathered data.
 */
lunasa::DataObject OpGather::createOutgoingMessage(faodel::nodeid_t dst,
                                                   const mailbox_t &src_mailbox,
                                                   const mailbox_t &dst_mailbox,
                                                   const uint32_t block_length,
                                                   const int target_rank) {
  gather_key key;
  int key_size = sizeof(gather_key);

  lunasa::DataObject ldo_msg = opbox::net::NewMessage(
          sizeof(message_t) + key_size + sizeof(struct opbox::net::NetBufferRemote));

  message_t *msg = ldo_msg.GetDataPtr<message_t *>();
  msg->src           = opbox::net::GetMyID();
  msg->dst           = dst;
  msg->src_mailbox   = src_mailbox;
  msg->dst_mailbox   = dst_mailbox;
  msg->op_id         = OpGather::op_id;
  msg->body_len      = key_size + sizeof(struct opbox::net::NetBufferRemote);

  opbox::net::NetBufferLocal *nbl = nullptr;
  opbox::net::NetBufferRemote nbr;
  // We don't want the target to reference the entire LDO, so we
  // specify an offset and length that creates a widow in the LDO.
  // Since a complete LDO includes the header, the offset must include
  // the size of the header.
  int rc = opbox::net::GetRdmaPtr(&gather_ldo, gather_ldo.GetHeaderSize() + (block_length*target_rank), block_length,
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
lunasa::DataObject OpGather::createAckMessage(faodel::nodeid_t dst,
                                              const mailbox_t &src_mailbox,
                                              const mailbox_t &dst_mailbox) {
  lunasa::DataObject ldo_msg = opbox::net::NewMessage(sizeof(message_t));
  auto *msg = ldo_msg.GetDataPtr<message_t *>();
  msg->src           = opbox::net::GetMyID();
  msg->dst           = dst;
  msg->src_mailbox   = src_mailbox;
  msg->dst_mailbox   = dst_mailbox;
  msg->op_id         = OpGather::op_id;
  msg->body_len      = 0;

  return ldo_msg;
}

/*
 * This is the origin state machine.
 */
WaitingType OpGather::UpdateOrigin(OpArgs *args) {
  uint32_t block_size;
  lunasa::DataObject ldo_msg;
  string user_data;

  switch(state) {
    case State::start:
      // evenly divide the gather destination amongst the targets
      block_size = gather_ldo.GetDataSize()/peer_count;
      // create and send the init message to each target
      for(int i = 0; i<peer_count; i++) {
        ldo_msg = createOutgoingMessage(opbox::net::ConvertPeerToNodeID(peer_list[i]),
                                        GetAssignedMailbox(),
                                        MAILBOX_UNSPECIFIED,
                                        block_size,
                                        i);

        // send a message to the target process to begin the gather
        opbox::net::SendMsg(peer_list[i], std::move(ldo_msg));
      }

      state = State::snd_wait_for_ack;
      return WaitingType::waiting_on_cq;

    case State::snd_wait_for_ack:
      // an ACK message has arrived
      args->VerifyTypeOrDie(UpdateType::incoming_message, op_name);

      // keep track of how many targets have sent an ACK
      gather_count++;
      // if we don't have all the ACKs, keep waiting
      if(gather_count<peer_count) {
        return WaitingType::waiting_on_cq;
      }

      // set the the value of the promise which will wake up the main
      // thread which is waiting on the associated future
      gather_promise.set_value(1);

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
WaitingType OpGather::UpdateTarget(OpArgs *args) {
  int key_size = sizeof(gather_key);

  switch(state) {

    case State::start: {
      auto incoming_msg = args->ExpectMessageOrDie<message_t *>(&origin);

      // save a copy of the key
      memcpy(key, &incoming_msg->body[0], key_size);
      // save a copy of the NBR for later use
      memcpy(&nbr, &incoming_msg->body[14], sizeof(opbox::net::NetBufferRemote));

      // retrieve the origins data using the key provided
      gather_ldo = examples::SimpleDataStore::Get(std::string(key));

      // we create the ACK message now be cause we need the sender's
      // node ID and the mailbox ID from the incoming message.
      // instead of saving the message, just use what we need.
      ack_msg = createAckMessage(incoming_msg->src,
                                 MAILBOX_UNSPECIFIED, //Not expecting a reply
                                 incoming_msg->src_mailbox);

      // put the origin's data (skipping the header) into the NBR they
      // sent us.
      // AllEventsCallback() is a convenience class that will redirect
      // all events generated by the get to this operation's Update()
      // method.
      opbox::net::Put(origin, gather_ldo, gather_ldo.GetHeaderSize(), &nbr, 0, nbr.GetLength(),
                      AllEventsCallback(this));

      state = State::put_wait_complete;
      return WaitingType::waiting_on_cq;
    }
    case State::put_wait_complete:

      // the put is complete, so send the ACK to the origin process.
      opbox::net::SendMsg(origin, std::move(ack_msg));

      state = State::done;
      return WaitingType::done_and_destroy;

    case State::done:
      return WaitingType::done_and_destroy;
  }
  KHALT("Missing state");
  return WaitingType::done_and_destroy;
}

string OpGather::GetStateName() const {
  switch(state){
  case State::start:             return "Start";
  case State::snd_wait_for_ack:  return "Sender-WaitForAck";
  case State::put_wait_complete: return "PutWaitComplete";
  case State::done:              return "Done";
  }
  KFAIL();
  return "Unknown";
}
