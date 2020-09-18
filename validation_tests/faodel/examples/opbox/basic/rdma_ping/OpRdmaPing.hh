// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#ifndef OPRDMAPING_HH
#define OPRDMAPING_HH

#include <future>

#include "opbox/OpBox.hh"
#include "lunasa/Lunasa.hh"
#include "lunasa/DataObject.hh"


class OpRdmaPing : public opbox::Op {

  enum class State : int  {
      start=0,
      snd_wait_for_ack,
      get_wait_complete,
      put_wait_complete,
      done };

public:
  OpRdmaPing(opbox::net::peer_ptr_t dst, lunasa::DataObject ping_ldo);
  OpRdmaPing(op_create_as_target_t t);
  ~OpRdmaPing();

  //Means for passing back the result
  std::future<std::string> GetFuture();

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

  std::promise<std::string> ping_promise;

  // we need to hold a copy of the NBR between calls to Update()
  opbox::net::NetBufferRemote nbr;

  lunasa::DataObject  ping_ldo;
  lunasa::DataObject  shout_ldo;
  lunasa::DataObject ldo_msg;

  void createOutgoingMessage(faodel::nodeid_t   dst,
                             const mailbox_t    &src_mailbox,
                             const mailbox_t    &dst_mailbox,
                             lunasa::DataObject  ping_ldo);
  void createAckMessage(faodel::nodeid_t   dst,
                        const mailbox_t    &src_mailbox,
                        const mailbox_t    &dst_mailbox);

};

#endif // OPRDMAPING_HH
