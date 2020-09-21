// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <assert.h>
#include <string.h>

#include "OpBenchmarkPing.hh"

using namespace std;

const unsigned int OpBenchmarkPing::op_id = const_hash("OpBenchmarkPing");
const string OpBenchmarkPing::op_name = "OpBenchmarkPing";


OpBenchmarkPing::OpBenchmarkPing(opbox::net::peer_ptr_t dst, uint32_t size, uint32_t count, uint32_t max_inflight)
        : faodel::LoggingInterface("Opbox", "OpBenchmarkPing"),
          state(State::start),
          size(size), count(count), max_inflight(max_inflight),
          started(0), inflight(0), completed(0),
          Op(true) {
  peer = dst;
  peer_node = opbox::net::ConvertPeerToNodeID(peer);
  timers = new std::vector<struct ping_timer>(count);
  //Work picks up again in origin's state machine
}

OpBenchmarkPing::OpBenchmarkPing(op_create_as_target_t t)
        : faodel::LoggingInterface("Opbox", "OpBenchmarkPing"),
          state(State::start),
          size(0), count(0), max_inflight(0),
          started(0), inflight(0), completed(0),
          Op(t) {
  //No work to do - done in target's state machine
}

OpBenchmarkPing::~OpBenchmarkPing() {
}

future <uint32_t> OpBenchmarkPing::GetFuture() {
  return promise.get_future();
}

void OpBenchmarkPing::createOutgoingMessage(lunasa::DataObject &ldo_msg,
                                            faodel::nodeid_t   dst,
                                            const mailbox_t    &src_mailbox,
                                            const mailbox_t    &dst_mailbox,
                                            const uint32_t      size,
                                            const uint32_t      id) {

  ldo_msg = opbox::net::NewMessage( sizeof(message_t)+size);
  message_t *msg = ldo_msg.GetDataPtr<message_t *>();
  msg->src           = opbox::net::GetMyID();
  msg->dst           = dst;
  msg->src_mailbox   = src_mailbox;
  msg->dst_mailbox   = dst_mailbox;
  msg->op_id         = OpBenchmarkPing::op_id;
  msg->body_len      = size;
  *((uint32_t*)&msg->body[0]) = count;
  *((uint32_t*)&msg->body[4]) = id;

  stringstream ss;
  ss << "create outgoing with id=" << id << std::endl;
  dbg(ss.str());
}

WaitingType OpBenchmarkPing::UpdateOrigin(OpArgs *args) {
  stringstream ss;
  lunasa::DataObject ldo_msg;

  uint32_t id;

  switch(state) {
    case State::start: {
      state = State::snd_wait_for_reply;

      timers->at(started).start = std::chrono::high_resolution_clock::now();
      createOutgoingMessage(ldo_msg,
                            peer_node,
                            GetAssignedMailbox(),
                            0,
                            size,
                            started);
      opbox::net::SendMsg(peer, std::move(ldo_msg));
      started++;
      inflight++;
      ss.str(std::string());
      ss << "inflight=" << inflight << "  started=" << started << "  completed=" << completed << std::endl;
      dbg(ss.str());

      return WaitingType::waiting_on_cq;
    }
    case State::snd_wait_for_reply: {
      auto incoming_msg = args->ExpectMessageOrDie<message_t *>(&peer);

      id = *((uint32_t*) & incoming_msg->body[4]);

      timers->at(id).end = std::chrono::high_resolution_clock::now();

      ss.str(std::string());
      ss << "id=" << id << std::endl;
      dbg(ss.str());

      completed++;
      inflight--;
      ss.str(std::string());
      ss << "inflight=" << inflight << "  max_inflight=" << max_inflight << "  completed=" << completed << std::endl;
      dbg(ss.str());

      while((inflight<max_inflight) && (started<count)) {
        timers->at(started).start = std::chrono::high_resolution_clock::now();
        createOutgoingMessage(ldo_msg,
                              peer_node,
                              GetAssignedMailbox(),
                              incoming_msg->src_mailbox,
                              size,
                              started);
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
          fprintf(stdout, "short_message chrono - %6lu   %6lu   %6lu   %6lu    %6luus\n",
                  size, i, start, end, per_ping_us);
        }
        uint64_t total_us = std::chrono::duration_cast<std::chrono::microseconds>(
                timers->at(id).end - timers->at(0).start).count();
        float total_sec = (float) total_us/1000000.0;
        fprintf(stdout, "short_message chrono - total - %6lu        %6lu    %6luus   %6.6fs\n",
                size, count, total_us, total_sec);

        promise.set_value(count);
        state = State::done;
        return WaitingType::done_and_destroy;
      }
      return WaitingType::waiting_on_cq;
    }
    case State::done:
      return WaitingType::done_and_destroy;
  }
  //Shouldn't be here
  //KFAIL();
  return WaitingType::error;
}

WaitingType OpBenchmarkPing::UpdateTarget(OpArgs *args) {
  lunasa::DataObject ldo_msg;
  uint32_t id;

  switch(state) {

    case State::start: {
      auto incoming_msg = args->ExpectMessageOrDie<message_t *>(&peer);

      count = *((uint32_t*) & incoming_msg->body[0]);
      id = *((uint32_t*) & incoming_msg->body[4]);

      createOutgoingMessage(ldo_msg,
                            incoming_msg->src,
                            GetAssignedMailbox(),
                            incoming_msg->src_mailbox,
                            incoming_msg->body_len,
                            id);
      opbox::net::SendMsg(peer, std::move(ldo_msg));

      state = State::tgt_created;
      return WaitingType::waiting_on_cq;
    }

    case State::tgt_created: {
      auto incoming_msg = args->ExpectMessageOrDie<message_t *>(&peer);

      id = *((uint32_t*) & incoming_msg->body[4]);

      createOutgoingMessage(ldo_msg,
                            incoming_msg->src,
                            GetAssignedMailbox(),
                            incoming_msg->src_mailbox,
                            incoming_msg->body_len,
                            id);
      opbox::net::SendMsg(peer, std::move(ldo_msg));

      if(id == count - 1) {
        state = State::done;
        return WaitingType::done_and_destroy;
      }

      return WaitingType::waiting_on_cq;
    }
    case State::done:
      return WaitingType::done_and_destroy;
  }
  KHALT("Missing state");
  return WaitingType::done_and_destroy;
}

string OpBenchmarkPing::GetStateName() const {
  switch(state){
      case State::start:              return "Start";
      case State::snd_wait_for_reply: return "Sender-WaitForReply";
      case State::done:               return "Done";
  }
  KFAIL();
  return "Unknown";
}
