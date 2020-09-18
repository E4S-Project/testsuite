// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#ifndef OPGATHER_HH
#define OPGATHER_HH

#include <future>

#include "opbox/OpBox.hh"
#include "lunasa/Lunasa.hh"
#include "lunasa/DataObject.hh"


class OpGather : public opbox::Op {

  enum class State : int  {
      start=0,
      snd_wait_for_ack,
      put_wait_complete,
      done };

  typedef char gather_key[14];

public:
  OpGather(opbox::net::peer_ptr_t *peer_list, int peer_count, lunasa::DataObject gather_src_ldo);
  OpGather(op_create_as_target_t t);
  ~OpGather();

  //Means for passing back the result
  std::future<int> GetFuture();

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
  // the process that initiated the gather
  opbox::net::peer_ptr_t  origin;
  // the list of peers to gather from
  opbox::net::peer_ptr_t *peer_list;
  // the number of peers in peer_list
  int              peer_count;

  int gather_count;

  std::promise<int> gather_promise;

  // we need to hold a copy of the NBR between calls to Update()
  opbox::net::NetBufferRemote nbr;

  // the key the origin wants us to use to find the gather data
  gather_key         key;
  // an LDO to reference the data the origin wants to gather
  lunasa::DataObject  gather_ldo;
  // an LDO used to send the ACK to the origin
  lunasa::DataObject ack_msg;

  lunasa::DataObject createOutgoingMessage(faodel::nodeid_t   dst,
                                            const mailbox_t    &src_mailbox,
                                            const mailbox_t    &dst_mailbox,
                                            const uint32_t      block_length,
                                            const int           target_rank);
  lunasa::DataObject createAckMessage(faodel::nodeid_t   dst,
                                       const mailbox_t    &src_mailbox,
                                       const mailbox_t    &dst_mailbox);

};

#endif // OPGATHER_HH
