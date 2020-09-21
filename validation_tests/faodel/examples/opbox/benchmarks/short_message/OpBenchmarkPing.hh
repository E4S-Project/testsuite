// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#ifndef OPBENCHMARKPING_HH
#define OPBENCHMARKPING_HH

#include <atomic>
#include <future>

#include "faodel-common/Common.hh"
#include "faodel-common/LoggingInterface.hh"

#include "opbox/OpBox.hh"
#include "lunasa/DataObject.hh"


class OpBenchmarkPing
    : public opbox::Op,
      public faodel::LoggingInterface {

    enum class State : int  {
        start=0,
        snd_wait_for_reply,
        tgt_created,
        done
    };

    struct ping_timer {
        std::chrono::high_resolution_clock::time_point start;
        std::chrono::high_resolution_clock::time_point end;
    };

public:
    OpBenchmarkPing(opbox::net::peer_ptr_t dst, uint32_t size, uint32_t count, uint32_t max_inflight);
    OpBenchmarkPing(op_create_as_target_t t);
    ~OpBenchmarkPing();

    //Means for passing back the result
    std::future<uint32_t> GetFuture();

    //Unique name and id for this op
    const static unsigned int op_id;
    const static std::string  op_name;
    unsigned int getOpID() const { return op_id; }
    std::string  getOpName() const { return op_name; }

    WaitingType UpdateOrigin(OpArgs *args);
    WaitingType UpdateTarget(OpArgs *args);

    std::string GetStateName() const;

private:
    State state;
    opbox::net::peer_t *peer;
    faodel::nodeid_t peer_node;

    uint32_t size;          // the payload size of each message
    uint32_t count;         // the number of messages this op must complete
    uint32_t max_inflight;  // the max number of outstanding messages

    uint32_t started;   // total number of messages sent
    uint32_t inflight;  // current number of outstanding message
    uint32_t completed; // number of message responses received

    std::vector<struct ping_timer> *timers;

    std::promise<uint32_t> promise;

    void createOutgoingMessage(lunasa::DataObject &ldo_msg,
                               faodel::nodeid_t   dst,
                               const mailbox_t    &src_mailbox,
                               const mailbox_t    &dst_mailbox,
                               const uint32_t      size,
                               const uint32_t      id);

};

#endif // OPBENCHMARKPING_HH
