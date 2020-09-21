// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include "WorkerThread.hh"

using namespace std;


WorkerThread::WorkerThread(int id, int num_timesteps)
  : id(id), needs_a_join(false), num_timesteps(num_timesteps) {
}

void WorkerThread::Start(){
  needs_a_join=true;
  th = thread(&WorkerThread::Run, this);
}
void WorkerThread::Join(){
  if(needs_a_join){
    th.join();
    needs_a_join=false;
  }
}
WorkerThread::~WorkerThread(){
  Join();
}
