#include <iostream>
#include <random>
#include "dmap-lpc.hpp"

#include <cstdlib>
#include <random>
#include <thread>

#if !UPCXX_BACKEND_GASNET_PAR
  #error "UPCXX_BACKEND=gasnet_par required."
#endif

using namespace std;

int main(int argc, char *argv[])
{
  upcxx::init();
  long N = 0;
  if (argc > 1) N = atol(argv[1]);
  if (N < 1) N = 100000;
  if (!upcxx::rank_me())
    cout << "Running persona-example on " << upcxx::rank_n() 
         << " procs with " << N << " iterations." << endl;

  DistrMap dmap;

  // insert all key, value pairs into the hash map, wait for operation to complete
  for (long i = 0; i < N; i++) {
    //use the key as value as well for simplicity
    string key = to_string(upcxx::rank_me()) + ":" + to_string(i);
    string val = key;
    upcxx::future<> fut = dmap.insert(key, val);
    // wait for operation to complete before next insert
    fut.wait();
  }
  // barrier to ensure all insertions have completed
  upcxx::barrier();

  //SNIPPET
  // try to fetch keys inserted by neighbor
  // note that in this example, keys and values are assumed to be the same
  const int num_threads = 10;
  thread * threads[num_threads];
  // declare an agreed upon persona for the progress thread
  upcxx::persona progress_persona;
  atomic<int> thread_barrier(0);
  int lpc_count = N;
  // liberate the master persona to allow the progress thread to use it
  upcxx::liberate_master_persona();
  // create a thread to execute the assertions while lpc_count is greater than 0
  thread progress_thread( [&]() {
      // push the master persona onto this thread's stack
      upcxx::persona_scope scope(upcxx::master_persona());
      // push the progress_persona as well
      upcxx::persona_scope progress_scope(progress_persona);
      // wait until all assertions in LPCs are complete
      while(lpc_count > 0) { 
        sched_yield();
        upcxx::progress();
      }
      cout<<"Progress thread on process "<<upcxx::rank_me()<<" is done"<<endl; 
      // unlock the other threads
      thread_barrier += 1;
      });
  // launch multiple threads to perform find operations
  for (int tid=0; tid<num_threads; tid++) {
    threads[tid] = new thread( [&,tid] () {
        // split the work across threads
        long num_asserts = N / num_threads;
        long i_beg = tid * num_asserts;
        long i_end = tid==num_threads-1?N:(tid+1)*num_asserts;
        for (long i = i_beg; i < i_end; i++) {
          string key = to_string((upcxx::rank_me() + 1) % upcxx::rank_n()) + ":" + to_string(i);
          // attach callback, which itself runs a LPC on progress_persona on completion
          dmap.find(key, progress_persona, 
              [key,&lpc_count](string val) {
                assert(val == key);
                lpc_count--;
              });
        }
        // block here until the progress thread has executed all RPCs and LPCs
        while(thread_barrier.load(memory_order_acquire) != 1){
          sched_yield();
          upcxx::progress();
        }
        });
  }

  // wait until all threads are done
  progress_thread.join();
  for (int tid=0; tid<num_threads; tid++) {
    threads[tid]->join();
    delete threads[tid];
  }
  {
    // push the master persona onto the initial thread's persona stack
    // before calling barrier and finalize
    upcxx::persona_scope scope(upcxx::master_persona());
    // wait until all processes are done
    upcxx::barrier();
    if (upcxx::rank_me() == 0 )
      cout<<"SUCCESS"<<endl;
    upcxx::finalize();
  }
  //SNIPPET
  return 0;
}


