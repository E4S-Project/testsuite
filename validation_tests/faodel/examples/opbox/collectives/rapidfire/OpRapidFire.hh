// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#ifndef OPRAPIDFIRE_HH
#define OPRAPIDFIRE_HH

#include <future>

#include "opbox/OpBox.hh"
#include "faodel-common/Common.hh"
#include "lunasa/DataObject.hh"


typedef struct {
  message_t hdr;
  int msg_id;
  int num_msgs;
  size_t getBodyLen() const { return 2*sizeof(int); }
  void ReuseMailbox(bool value) { hdr.user_flags = 0x01; }
  bool ReuseMailbox() { return (hdr.user_flags & 0x01); }
} rapidfire_message_t;

class OpRapidFire : public opbox::Op {

  enum class State : int  {
      start=0,
      snd_OneAtATime,
      done };


public:
  OpRapidFire(faodel::nodeid_t dst_node, int num_messages, bool dst_reuse_mailbox);
  OpRapidFire(op_create_as_target_t t);
  ~OpRapidFire();

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
  mailbox_t my_mailbox;
  faodel::nodeid_t dst_node;
  int messages_sent;
  int num_messages;
  opbox::net::peer_t *peer;
  lunasa::DataObject ldo_msg;
  bool dst_reuse_mailbox;

  std::promise<int> rf_promise;

  void createOutgoingMessage(faodel::nodeid_t receiver_node,
                             const opbox::mailbox_t &sender_mailbox,
                             const opbox::mailbox_t &reveiver_mailbox,
                             int message_id,
                             int num_messages,
                             bool reuse_mailbox);


};



#endif // OPRINGER_HH
