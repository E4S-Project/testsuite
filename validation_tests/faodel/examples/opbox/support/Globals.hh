// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#ifndef FAODEL_EXAMPLES_OPBOX_GLOBALS_HH
#define FAODEL_EXAMPLES_OPBOX_GLOBALS_HH

#include <mpi.h>

#include "faodel-common/Common.hh"
#include "opbox/OpBox.hh"
#include "whookie/Server.hh"


class Globals {

public:
  Globals();
  ~Globals();

  int mpi_rank;
  int mpi_size;
  faodel::nodeid_t myid;
  faodel::nodeid_t *nodes;
  opbox::net::peer_ptr_t *peers;
  int debug_level;

  void StartAll(int &argc, char **argv, faodel::Configuration &config);
  void StopAll();
  void dump();
  void log(std::string s);
  void dbg(std::string s);
};


#endif // FAODEL_EXAMPLES_OPBOX_GLOBALS_HH
