// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <assert.h>
#include <string.h>

#include "opbox/ops/OpHelpers.hh"

#include "OpBenchmarkMsgRdma.hh"

using namespace std;

const unsigned int OpBenchmarkMsgRdma::op_id = const_hash("OpBenchmarkMsgRdma");
const string OpBenchmarkMsgRdma::op_name = "OpBenchmarkMsgRdma";


OpBenchmarkMsgRdma::OpBenchmarkMsgRdma(opbox::net::peer_ptr_t dst, uint32_t size, uint32_t count, uint32_t max_inflight)
        : faodel::LoggingInterface("Opbox", "OpBenchmarkMsgRdma"),
          state(State::start),
          size(size), count(count), max_inflight(max_inflight),
          started(0), inflight(0), completed(0),
          Op(true) {
  peer = dst;
  rdma_ldo = lunasa::DataObject(0, size, lunasa::DataObject::AllocatorType::eager);
  //Work picks up again in origin's state machine
}

OpBenchmarkMsgRdma::OpBenchmarkMsgRdma(op_create_as_target_t t)
        : faodel::LoggingInterface("Opbox", "OpBenchmarkMsgRdma"),
          state(State::start),
          size(0), count(0), max_inflight(0),
          started(0), inflight(0), completed(0),
          Op(t) {
  //No work to do - done in target's state machine
}

OpBenchmarkMsgRdma::~OpBenchmarkMsgRdma() {
}

future <uint32_t> OpBenchmarkMsgRdma::GetFuture() {
  return promise.get_future();
}

/*
 * Create a message to initiate the communication with the target.
 * This message starts with a message_t that carries basic
 * information about the origin (us) and the target (them).  Following
 * the message_t is the body of the message.  The body of this
 * message is a NetBufferRemote that describes the RDMA window of the
 * ping message.
 */
void OpBenchmarkMsgRdma::createOutgoingMessage(faodel::nodeid_t dst,
                                               const mailbox_t &src_mailbox,
                                               const mailbox_t &dst_mailbox) {
  ldo_msg = opbox::net::NewMessage(sizeof(message_t) + sizeof(struct bench_params));
  message_t *msg = ldo_msg.GetDataPtr<message_t *>();
  msg->src           = opbox::net::GetMyID();
  msg->dst           = dst;
  msg->src_mailbox   = src_mailbox;
  msg->dst_mailbox   = dst_mailbox;
  msg->op_id         = OpBenchmarkMsgRdma::op_id;
  msg->body_len      = sizeof(struct bench_params);

  opbox::net::NetBufferLocal *nbl = nullptr;
  opbox::net::NetBufferRemote nbr;
  opbox::net::GetRdmaPtr(&rdma_ldo, &nbl, &nbr);

  bp = (struct bench_params *) &msg->body[0];
  bp->size = size;
  bp->count = count;
  bp->max_inflight = max_inflight;

  memcpy(&bp->nbr, &nbr, sizeof(opbox::net::NetBufferRemote));
}

void OpBenchmarkMsgRdma::updateOutgoingMessage(const mailbox_t &dst_mailbox) {
  message_t *msg = ldo_msg.GetDataPtr<message_t *>();
  msg->dst_mailbox = dst_mailbox;
}

/*
 * Create a message to terminate communication with the origin.  This
 * message is just a message_t that tells the origin (them) which
 * operation is complete.
 */
void OpBenchmarkMsgRdma::createAckMessage(faodel::nodeid_t dst,
                                          const mailbox_t &src_mailbox,
                                          const mailbox_t &dst_mailbox) {
  ldo_msg = opbox::net::NewMessage(sizeof(message_t));
  message_t *msg = ldo_msg.GetDataPtr<message_t *>();
  msg->src           = opbox::net::GetMyID();
  msg->dst           = dst;
  msg->src_mailbox   = src_mailbox;
  msg->dst_mailbox   = dst_mailbox;
  msg->op_id         = OpBenchmarkMsgRdma::op_id;
  msg->body_len      = 0;
}

WaitingType OpBenchmarkMsgRdma::UpdateOrigin(OpArgs *args) {
  stringstream ss;

  switch(state) {
    case State::start:
      /*
       * Even if max_inflight is greater than 1, we can only
       * send one message to start, because we must wait for the
       * first response in order to know the targets mailbox.
       * If we don't wait for the mailbox, the target will create
       * a new Op for each incoming message.
       */

      createOutgoingMessage(opbox::net::ConvertPeerToNodeID(peer),
                            GetAssignedMailbox(),
                            0);  // 0 means this is a new target Op

      timers = new std::vector<struct ping_timer>(count);

      state = State::snd_wait_for_ack;

      timers->at(started).start = std::chrono::high_resolution_clock::now();
      // send a message to the target process to begin
      opbox::net::SendMsg(peer, std::move(ldo_msg));
      started++;
      inflight++;
      ss.str(std::string());
      ss << "inflight=" << inflight << "  started=" << started << "  completed=" << completed << std::endl;
      dbg(ss.str());

      return WaitingType::waiting_on_cq;

    case State::snd_wait_for_ack: {
      timers->at(completed).end = std::chrono::high_resolution_clock::now();
      completed++;
      inflight--;
      ss.str(std::string());
      ss << "inflight=" << inflight << "  max_inflight=" << max_inflight << "  completed=" << completed << std::endl;
      dbg(ss.str());

      // update our outgoing message with the target's mailbox.
      // this ensures that we talk to the same target Op in the future.
      auto incoming_msg = args->ExpectMessageOrDie<message_t *>();

      // now we can send a burst of messages because wwe know the destination mailbox
      while((inflight<max_inflight) && (started<count)) {
        timers->at(started).start = std::chrono::high_resolution_clock::now();

        createOutgoingMessage(opbox::net::ConvertPeerToNodeID(peer),
                              GetAssignedMailbox(),
                              incoming_msg->src_mailbox);

        // send a message to the target process to begin the ping.
        opbox::net::SendMsg(peer, std::move(ldo_msg));
        started++;
        inflight++;
        ss.str(std::string());
        ss << "inflight=" << inflight << "  started=" << started << "  completed=" << completed << std::endl;
        dbg(ss.str());
      }

      if(completed == count) {
        for(int i = 0; i<count; i++) {
          uint64_t end = std::chrono::duration_cast<std::chrono::microseconds>(
                  timers->at(i).end.time_since_epoch()).count();
          uint64_t start = std::chrono::duration_cast<std::chrono::microseconds>(
                  timers->at(i).start.time_since_epoch()).count();
          uint64_t per_ping_us = end - start;
          fprintf(stdout, "rdma Msg+RDMA chrono - %6lu   %6lu   %6lu   %6lu    %6luus\n",
                  size, i, start, end, per_ping_us);
        }
        uint64_t total_us = std::chrono::duration_cast<std::chrono::microseconds>(
                timers->at(completed - 1).end - timers->at(0).start).count();
        float total_sec = (float) total_us/1000000.0;
        fprintf(stdout, "rdma Msg+RDMA chrono - total - %6lu        %6lu    %6luus   %6.6fs\n",
                size, count, total_us, total_sec);

        // set the the value of the promise which will wake up the main
        // thread which is waiting on the associated future
        promise.set_value(count);

        dbg("setting origin to done state");
        state = State::done;
        return WaitingType::done_and_destroy;
      }

      return WaitingType::waiting_on_cq;
    }
    case State::done:
      return WaitingType::done_and_destroy;
  }
  //Shouldn't be here
  KHALT("Missing state");
  return WaitingType::done_and_destroy;
}

WaitingType OpBenchmarkMsgRdma::UpdateTarget(OpArgs *args) {
  stringstream ss;
  ss.str(std::string());
  //ss << "incoming_msg->src_mailbox=" << incoming_msg->src_mailbox;
  //dbg(ss.str());

  switch(state) {

    case State::start: {
      auto incoming_msg = args->ExpectMessageOrDie<message_t *>(&peer);
      state = State::tgt_created;

      bp = (struct bench_params *) &incoming_msg->body[0];
      size = bp->size;
      count = bp->count;
      max_inflight = bp->max_inflight;

      memcpy(&nbr, &bp->nbr, sizeof(opbox::net::NetBufferRemote));

      // this is the initiator buffer for the get.
      // allocates enough space to avoid overlapping gets.
      rdma_ldo = lunasa::DataObject(0, nbr.GetLength()*count, lunasa::DataObject::AllocatorType::eager);

      src_nodeid = incoming_msg->src;
      src_mailbox = incoming_msg->src_mailbox;

      // fall through here.  future events will jump directly here.
    }
    case State::tgt_created:
      // the ACK message has arrived
      if(args->type == UpdateType::incoming_message) {
        opbox::net::Get(peer, &nbr, 0, rdma_ldo, size*started, size, AllEventsCallback(this));
        started++;

      } else if(args->type == UpdateType::get_success) {
        // the GET is complete, so send the ACK to the origin process.
        createAckMessage(src_nodeid,
                         GetAssignedMailbox(),
                         src_mailbox);
        opbox::net::SendMsg(peer, std::move(ldo_msg));
        completed++;
        ss.str(std::string());
        ss << "completed=" << completed << "  started=" << started;
        dbg(ss.str());
        if(completed == count) {
          dbg("setting target to done state");
          state = State::done;
          return WaitingType::done_and_destroy;
        }
      } else {
        // anything else is an error
        abort();
      }

      return WaitingType::waiting_on_cq;

    case State::done:
      return WaitingType::done_and_destroy;
  }
  KHALT("Missing state");
  return WaitingType::done_and_destroy;
}

string OpBenchmarkMsgRdma::GetStateName() const {
  switch(state){
      case State::start:            return "Start";
      case State::snd_wait_for_ack: return "Sender-WaitForAck";
      case State::done:             return "Done";
  }
  KFAIL();
  return "Unknown";
}
