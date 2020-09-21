// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#ifndef KELPIE_DIMSUM_WORKERTHREAD_HH
#define KELPIE_DIMSUM_WORKERTHREAD_HH

#include <thread>

class WorkerThread {
public:
  WorkerThread(int id, int num_timesteps);
  virtual ~WorkerThread();

  void Start();
  virtual void Run()=0;
  void Join();

protected:
  int id;
  int num_timesteps;
  bool needs_a_join;
  std::thread th;
};

#endif // KELPIE_DIMSUM_WORKERTHREAD_HH
