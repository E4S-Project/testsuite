// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#ifndef OPBENCHMARKFETCHADD_HH
#define OPBENCHMARKFETCHADD_HH

#include <atomic>
#include <future>

#include "faodel-common/LoggingInterface.hh"

#include "opbox/OpBox.hh"
#include "lunasa/Lunasa.hh"
#include "lunasa/DataObject.hh"


class OpBenchmarkFetchAdd
    : public opbox::Op,
      public faodel::LoggingInterface {

    enum class State : int  {
        start=0,
        snd_wait_for_ack,
        tgt_created,
        done
    };

    struct ping_timer {
        std::chrono::high_resolution_clock::time_point start;
        std::chrono::high_resolution_clock::time_point end;
    };

public:
    OpBenchmarkFetchAdd(opbox::net::peer_ptr_t dst, uint32_t count, uint32_t max_inflight);
    OpBenchmarkFetchAdd(op_create_as_target_t t);
    ~OpBenchmarkFetchAdd();

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
    struct bench_params {
        uint32_t count;                  // the number of messages this op must complete
        uint32_t max_inflight;           // the max number of outstanding messages
        struct opbox::net::NetBufferRemote nbr; // the target of the atomic operation
    } *bp;

    State state;
    opbox::net::peer_t *peer;

    uint32_t count;         // the number of atomics this op must complete
    uint32_t max_inflight;  // the max number of outstanding atomic operations

    uint32_t started;   // total number of atomic operations
    uint32_t inflight;  // current number of outstanding atomic operations
    uint32_t completed; // number of atomic operation responses received

    // we need to hold a copy of the NBR between calls to Update()
    opbox::net::NetBufferRemote nbr;

    lunasa::DataObject atomics_ldo;

    lunasa::DataObject ldo_msg;

    std::atomic<bool> first_burst_sent;

    std::vector<struct ping_timer> *timers;

    std::promise<uint32_t> promise;

    void createOutgoingMessage(faodel::nodeid_t   dst,
                               const mailbox_t    &src_mailbox,
                               const mailbox_t    &dst_mailbox);
    void createAckMessage(faodel::nodeid_t   dst,
                          const mailbox_t    &src_mailbox,
                          const mailbox_t    &dst_mailbox);

};

#endif // OPBENCHMARKFETCHADD_HH
