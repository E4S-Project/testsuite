// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <assert.h>
#include <string.h>

#include "opbox/ops/OpHelpers.hh"

#include "OpBenchmarkFetchAdd.hh"

using namespace std;

const unsigned int OpBenchmarkFetchAdd::op_id = const_hash("OpBenchmarkFetchAdd");
const string OpBenchmarkFetchAdd::op_name = "OpBenchmarkFetchAdd";


OpBenchmarkFetchAdd::OpBenchmarkFetchAdd(opbox::net::peer_ptr_t dst, uint32_t count, uint32_t max_inflight)
        : faodel::LoggingInterface("Opbox", "OpBenchmarkFetchAdd"),
          state(State::start),
          count(count), max_inflight(max_inflight),
          started(0), inflight(0), completed(0),
          first_burst_sent(false),
          Op(true) {
  peer = dst;
  atomics_ldo = lunasa::DataObject(0, 8, lunasa::DataObject::AllocatorType::eager);
  memset(atomics_ldo.GetDataPtr(), 0, 8);
  //Work picks up again in origin's state machine
}

OpBenchmarkFetchAdd::OpBenchmarkFetchAdd(op_create_as_target_t t)
        : faodel::LoggingInterface("Opbox", "OpBenchmarkFetchAdd"),
          state(State::start),
          count(0), max_inflight(0),
          started(0), inflight(0), completed(0),
          first_burst_sent(false),
          Op(t) {
  //No work to do - done in target's state machine
}

OpBenchmarkFetchAdd::~OpBenchmarkFetchAdd() {
}

future <uint32_t> OpBenchmarkFetchAdd::GetFuture() {
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
void OpBenchmarkFetchAdd::createOutgoingMessage(faodel::nodeid_t dst,
                                                const mailbox_t &src_mailbox,
                                                const mailbox_t &dst_mailbox) {
  ldo_msg = opbox::net::NewMessage(sizeof(message_t) + sizeof(struct bench_params));
  message_t *msg = ldo_msg.GetDataPtr<message_t *>();
  msg->src           = opbox::net::GetMyID();
  msg->dst           = dst;
  msg->src_mailbox   = src_mailbox;
  msg->dst_mailbox   = dst_mailbox;
  msg->op_id         = OpBenchmarkFetchAdd::op_id;
  msg->body_len      = sizeof(struct bench_params);

  opbox::net::NetBufferLocal *nbl = nullptr;
  opbox::net::NetBufferRemote nbr;
  opbox::net::GetRdmaPtr(&atomics_ldo, atomics_ldo.GetHeaderSize() + atomics_ldo.GetMetaSize(), 8, &nbl, &nbr);

  bp = (struct bench_params *) &msg->body[0];
  bp->count = count;
  bp->max_inflight = max_inflight;

  memcpy(&bp->nbr, &nbr, sizeof(opbox::net::NetBufferRemote));
}

/*
 * Create a message to terminate communication with the origin.  This
 * message is just a message_t that tells the origin (them) which
 * operation is complete.
 */
void OpBenchmarkFetchAdd::createAckMessage(faodel::nodeid_t dst,
                                           const mailbox_t &src_mailbox,
                                           const mailbox_t &dst_mailbox) {
  ldo_msg = opbox::net::NewMessage(sizeof(message_t));
  message_t *msg = ldo_msg.GetDataPtr<message_t *>();
  msg->src           = opbox::net::GetMyID();
  msg->dst           = dst;
  msg->src_mailbox   = src_mailbox;
  msg->dst_mailbox   = dst_mailbox;
  msg->op_id         = OpBenchmarkFetchAdd::op_id;
  msg->body_len      = 0;
}

WaitingType OpBenchmarkFetchAdd::UpdateOrigin(OpArgs *args) {
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
      args->VerifyTypeOrDie(UpdateType::incoming_message, "FetchAdd");

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

WaitingType OpBenchmarkFetchAdd::UpdateTarget(OpArgs *args) {
  stringstream ss;

  switch(state) {

    case State::start: {
      auto incoming_msg = args->ExpectMessageOrDie<message_t *>(&peer);

      bp = (struct bench_params *) &incoming_msg->body[0];
      count = bp->count;
      max_inflight = bp->max_inflight;

      memcpy(&nbr, &bp->nbr, sizeof(opbox::net::NetBufferRemote));

      // this is the initiator buffer for the get and the put
      atomics_ldo = lunasa::DataObject(0, nbr.GetLength(), lunasa::DataObject::AllocatorType::eager);

      timers = new std::vector<struct ping_timer>(count);

      // we create the ACK message now be cause we need the sender's
      // node ID and the mailbox ID from the incoming message.
      // instead of saving the message, just use what we need.
      createAckMessage(incoming_msg->src,
                       0, //Not expecting a reply
                       incoming_msg->src_mailbox);

      state = State::tgt_created;

      while(started<max_inflight) {
        timers->at(started).start = std::chrono::high_resolution_clock::now();
        opbox::net::Atomic(peer, opbox::net::AtomicOp::FetchAdd, atomics_ldo,
                           atomics_ldo.GetHeaderSize() + atomics_ldo.GetMetaSize(), &nbr,
                           0, 8, 10, AllEventsCallback(this));
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
      timers->at(completed).end = std::chrono::high_resolution_clock::now();
      completed++;
      inflight--;
      ss.str(std::string());
      ss << "inflight=" << inflight << "  max_inflight=" << max_inflight << "  completed=" << completed << std::endl;
      dbg(ss.str());

      while((inflight<max_inflight) && (started<count) && first_burst_sent) {
        timers->at(started).start = std::chrono::high_resolution_clock::now();
        opbox::net::Atomic(peer, opbox::net::AtomicOp::FetchAdd, atomics_ldo,
                           atomics_ldo.GetHeaderSize() + atomics_ldo.GetMetaSize(), &nbr,
                           0, 8, 10, AllEventsCallback(this));
        started++;
        inflight++;
        ss.str(std::string());
        ss << "inflight=" << inflight << "  started=" << started << "  completed=" << completed << std::endl;
        dbg(ss.str());
      }

      if(completed == count) {
        uint64_t final = *atomics_ldo.GetDataPtr<uint64_t *>();
        fprintf(stdout, "rdma ATOMIC - final %6lu\n", final);

        for(int i = 0; i<count; i++) {
          uint64_t end = std::chrono::duration_cast<std::chrono::microseconds>(
                  timers->at(i).end.time_since_epoch()).count();
          uint64_t start = std::chrono::duration_cast<std::chrono::microseconds>(
                  timers->at(i).start.time_since_epoch()).count();
          uint64_t per_ping_us = end - start;
          fprintf(stdout, "rdma ATOMIC chrono - %6lu   %6lu   %6lu    %6luus\n",
                  i, start, end, per_ping_us);
        }
        uint64_t total_us = std::chrono::duration_cast<std::chrono::microseconds>(
                timers->at(completed - 1).end - timers->at(0).start).count();
        float total_sec = (float) total_us/1000000.0;
        fprintf(stdout, "rdma ATOMIC chrono - total - %6lu    %6luus   %6.6fs\n",
                count, total_us, total_sec);

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

string OpBenchmarkFetchAdd::GetStateName() const {
  switch(state){
      case State::start:            return "Start";
      case State::snd_wait_for_ack: return "Sender-WaitForAck";
      case State::done:             return "Done";
  }
  KFAIL();
  return "Unknown";
}
