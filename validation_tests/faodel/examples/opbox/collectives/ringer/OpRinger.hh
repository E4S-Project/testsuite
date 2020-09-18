// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#ifndef OPRINGER_HH
#define OPRINGER_HH

#include <future>

#include "opbox/OpBox.hh"
#include "lunasa/DataObject.hh"

#include "RingInfo.hh"

class OpRinger : public opbox::Op {

  enum class State : int  {
      start=0,
      snd_wait_for_reply,
      done };

public:
  OpRinger(RingInfo ring_info);
  OpRinger(op_create_as_target_t t);
  ~OpRinger();

  //Means for passing back the result
  std::future<RingInfo> GetFuture();

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
  lunasa::DataObject ldo_msg;

  std::promise<RingInfo> ring_promise;

};



#endif // OPRINGER_HH
