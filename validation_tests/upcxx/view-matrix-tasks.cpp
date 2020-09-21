#include "view-matrix-tasks.hpp"
#include <atomic>
#include <iostream>
#include <memory>
#include <random>
#include <thread>

int main() {
  upcxx::init();
  
  //////////////////////////////////////////////////////////////////////////////
  // Create worker threads
  
  std::thread *threads[worker_n];
  std::atomic<bool> shutdown{false};
  
  auto worker_main = [&shutdown](int me) {
    // Attach to respective persona.
    upcxx::persona_scope scope(workers[me]);
    
    while(!shutdown.load(std::memory_order_relaxed))
      upcxx::progress();
  };
  
  for(int t=0; t < worker_n; t++)
    threads[t] = new std::thread(worker_main, t);
  
  //////////////////////////////////////////////////////////////////////////////
  // Create random elements, all have value=1
  
  std::unique_ptr<element[]> elts(new element[1000]);
  std::default_random_engine rng(0xdeadbeef*upcxx::rank_me());
  
  for(int e=0; e < 1000; e++) {
    elts[e].row = rng() % 1000;
    elts[e].col = rng() % 1000;
    elts[e].value = 1;
  }
  
  //////////////////////////////////////////////////////////////////////////////
  // Send elements to some neighbors.
  
  int countdown = 10;
  
  for(int i=0; i < 10; i++) {
    int nebr = rng() % upcxx::rank_n();
    update_remote_matrix(nebr, elts.get(), 1000)
      .then([&]() { countdown -= 1; });
  }
  
  while(countdown != 0)
    upcxx::progress();
  upcxx::barrier();
  
  //////////////////////////////////////////////////////////////////////////////
  // Sum up matrix elements.
  
  int sum = 0;
  for(int r=0; r < 1000; r++)
    for(int c=0; c < 1000; c++)
      sum += (int)my_matrix[r][c];
  
  sum = upcxx::reduce_all(sum, upcxx::op_fast_add).wait();
  
  // 1000 elements with value=1, from 10 neighbors, across all processes.
  assert(sum == 1000*10*upcxx::rank_n());
  
  if(upcxx::rank_me()==0)
    std::cout<<"SUCCESS\n";
  
  //////////////////////////////////////////////////////////////////////////////
  // Shutdown workers.
  
  shutdown.store(true);
  for(int t=0; t < worker_n; t++) {
    threads[t]->join();
    delete threads[t];
  }
  
  upcxx::finalize();
  return 0;
}
