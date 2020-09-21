// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <assert.h>
#include <string.h>

#include "opbox/ops/OpHelpers.hh"

#include "OpBenchmarkPut.hh"

using namespace std;

const unsigned int OpBenchmarkPut::op_id = const_hash("OpBenchmarkPut");
const string OpBenchmarkPut::op_name = "OpBenchmarkPut";


OpBenchmarkPut::OpBenchmarkPut(opbox::net::peer_ptr_t dst, uint32_t size, uint32_t count, uint32_t max_inflight)
        : faodel::LoggingInterface("Opbox", "OpBenchmarkPut"),
          state(State::start),
          warmup_count(10),
          size(size), count(count), max_inflight(max_inflight),
          started(0), inflight(0), completed(0),
          first_burst_sent(false),
          Op(true) {
  peer = dst;
  rdma_ldo = lunasa::DataObject(0, size, lunasa::DataObject::AllocatorType::eager);
  //Work picks up again in origin's state machine
}

OpBenchmarkPut::OpBenchmarkPut(op_create_as_target_t t)
        : faodel::LoggingInterface("Opbox", "OpBenchmarkPut"),
          state(State::start),
          warmup_count(0),
          size(0), count(0), max_inflight(0),
          started(0), inflight(0), completed(0),
          first_burst_sent(false),
          Op(t) {
  //No work to do - done in target's state machine
}

OpBenchmarkPut::~OpBenchmarkPut() {
}

future <uint32_t> OpBenchmarkPut::GetFuture() {
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
void OpBenchmarkPut::createOutgoingMessage(faodel::nodeid_t dst,
                                           const mailbox_t &src_mailbox,
                                           const mailbox_t &dst_mailbox) {
  ldo_msg = opbox::net::NewMessage(sizeof(message_t) + sizeof(struct bench_params));
  message_t *msg = ldo_msg.GetDataPtr<message_t *>();
  msg->src           = opbox::net::GetMyID();
  msg->dst           = dst;
  msg->src_mailbox   = src_mailbox;
  msg->dst_mailbox   = dst_mailbox;
  msg->op_id         = OpBenchmarkPut::op_id;
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

/*
 * Create a message to terminate communication with the origin.  This
 * message is just a message_t that tells the origin (them) which
 * operation is complete.
 */
void OpBenchmarkPut::createAckMessage(faodel::nodeid_t dst,
                                      const mailbox_t &src_mailbox,
                                      const mailbox_t &dst_mailbox) {
  ldo_msg = opbox::net::NewMessage(sizeof(message_t));
  message_t *msg = ldo_msg.GetDataPtr<message_t *>();
  msg->src           = opbox::net::GetMyID();
  msg->dst           = dst;
  msg->src_mailbox   = src_mailbox;
  msg->dst_mailbox   = dst_mailbox;
  msg->op_id         = OpBenchmarkPut::op_id;
  msg->body_len      = 0;
}

WaitingType OpBenchmarkPut::UpdateOrigin(OpArgs *args) {
  switch(state) {
    case State::start:
      createOutgoingMessage(opbox::net::ConvertPeerToNodeID(peer),
                            GetAssignedMailbox(),
                            0);

      // send a message to the target process to begin the ping.
      opbox::net::SendMsg(peer, std::move(ldo_msg));

      state = State::snd_wait_for_ack;
      return WaitingType::waiting_on_cq;

    case State::snd_wait_for_ack:
      // the ACK message has arrived
      args->ExpectMessageOrDie<message_t *>();

      // set the the value of the promise which will wake up the main
      // thread which is waiting on the associated future
      promise.set_value(count);

      state = State::done;
      return WaitingType::done_and_destroy;

    case State::done:
      return WaitingType::done_and_destroy;
  }
  //Shouldn't be here
  KFAIL();
  return WaitingType::error;
}

WaitingType OpBenchmarkPut::UpdateTarget(OpArgs *args) {
  stringstream ss;


  switch(state) {

    case State::start: {
      auto incoming_msg = args->ExpectMessageOrDie<message_t *>(&peer);

      bp = (struct bench_params *) &incoming_msg->body[0];
      size = bp->size;
      count = bp->count;
      max_inflight = bp->max_inflight;

      memcpy(&nbr, &bp->nbr, sizeof(opbox::net::NetBufferRemote));

      // this is the initiator buffer for the get and the put
      rdma_ldo = lunasa::DataObject(0, nbr.GetLength(), lunasa::DataObject::AllocatorType::eager);

      timers = new std::vector<struct ping_timer>(count);

      // we create the ACK message now be cause we need the sender's
      // node ID and the mailbox ID from the incoming message.
      // instead of saving the message, just use what we need.
      createAckMessage(incoming_msg->src,
                       0, //Not expecting a reply
                       incoming_msg->src_mailbox);

      for (int i=0;i<warmup_count;i++) {
          opbox::net::Put(peer, rdma_ldo, &nbr, AllEventsCallback(this));
      }

      state = State::tgt_created;

      while((inflight<max_inflight) && ((inflight + completed)<count)) {
        timers->at(started).start = std::chrono::high_resolution_clock::now();
        opbox::net::Put(peer, rdma_ldo, &nbr, AllEventsCallback(this));
        started++;
        inflight++;
        ss.str(std::string());
        ss << "inflight=" << inflight << "  started=" << started << "  completed=" << completed << std::endl;
        dbg(ss.str());
      }
      first_burst_sent = true;

      ss.str(std::string());
      ss << "count=" << count << std::endl;
      dbg(ss.str());

      return WaitingType::waiting_on_cq;
    }
    case State::tgt_created:
      //TODO: Should there be a check for args type here?
      timers->at(completed).end = std::chrono::high_resolution_clock::now();
      completed++;
      inflight--;
      ss.str(std::string());
      ss << "inflight=" << inflight << "  max_inflight=" << max_inflight << "  completed=" << completed << std::endl;
      dbg(ss.str());

      while((inflight<max_inflight) && ((inflight + completed)<count) && first_burst_sent) {
        timers->at(started).start = std::chrono::high_resolution_clock::now();
        opbox::net::Put(peer, rdma_ldo, &nbr, AllEventsCallback(this));
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
          fprintf(stdout, "rdma PUT chrono - %6lu   %6lu   %6lu   %6lu    %6luus\n",
                  size, i, start, end, per_ping_us);
        }
        uint64_t total_us = std::chrono::duration_cast<std::chrono::microseconds>(
                timers->at(completed - 1).end - timers->at(0).start).count();
        float total_sec = (float) total_us/1000000.0;
        fprintf(stdout, "rdma PUT chrono - total - %6lu        %6lu    %6luus   %6.6fs\n",
                size, count, total_us, total_sec);

        promise.set_value(count);

        // the RDMAs are complete, so send the ACK to the origin process.
        opbox::net::SendMsg(peer, std::move(ldo_msg));

        state = State::done;
        return WaitingType::done_and_destroy;
      }

      return WaitingType::waiting_on_cq;

    case State::done:
      return WaitingType::done_and_destroy;
  }
  KHALT("Missing state");
  return WaitingType::done_and_destroy;
}

string OpBenchmarkPut::GetStateName() const {
  switch(state){
      case State::start:            return "Start";
      case State::snd_wait_for_ack: return "Sender-WaitForAck";
      case State::done:             return "Done";
  }
  KFAIL();
  return "Unknown";
}
