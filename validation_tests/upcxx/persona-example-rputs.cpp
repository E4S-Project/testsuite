#include <upcxx/upcxx.hpp> 


#include <thread>
#include <vector> 

#include <sched.h>

#if !UPCXX_BACKEND_GASNET_PAR
  #error "UPCXX_BACKEND=gasnet_par required."
#endif

using namespace std; 

//SNIPPET
int main () {
  upcxx::init(); 
  // create a landing zone, and share it through a dist_object
  // allocate and initialize with local rank
  upcxx::dist_object<upcxx::global_ptr<int>> dptrs(upcxx::new_<int>(upcxx::rank_me()));
  upcxx::global_ptr<int> my_ptr = *dptrs;
  // fetch my neighbor's pointer to its landing zone
  upcxx::intrank_t neigh_rank = (upcxx::rank_me() + 1)%upcxx::rank_n();
  upcxx::global_ptr<int> neigh_ptr = dptrs.fetch(neigh_rank).wait();
  // declare an agreed upon persona for the progress thread
  upcxx::persona progress_persona;
  atomic<int> thread_barrier(0);
  bool done = false;
  // create the progress thread
  thread progress_thread( [&]() {
        // push progress_persona onto this thread's persona stack
        upcxx::persona_scope scope(progress_persona);
        // progress thread drains progress until work is done
        while (!done)
          upcxx::progress();
        cout<<"Progress thread on process "<<upcxx::rank_me()<<" is done"<<endl; 
        //unlock the other threads
        thread_barrier += 1;
      });
  // create another thread to issue the rget
  thread submit_thread( [&]() {  
      // create a completion object to execute a LPC on the progress_thread
      // which verifies that the value we got was the rank of our neighbor
      auto cx = upcxx::operation_cx::as_lpc( progress_persona, [&done, neigh_rank](int got) {
              assert(got == neigh_rank);
              //signal that work is complete
              done = true;
            });
      // use this completion object on the rget
      upcxx::rget(neigh_ptr, cx);
      // block here until the progress thread has executed all LPCs
      while(thread_barrier.load(memory_order_acquire) != 1){
        sched_yield();
        upcxx::progress();
      }
    });
  // wait until all threads finish their work
  submit_thread.join();
  progress_thread.join();
  // wait until all processes are done
  upcxx::barrier();
  if ( upcxx::rank_me()==0 )
    cout<<"SUCCESS"<<endl;
  // delete my landing zone 
  upcxx::delete_(my_ptr);
  upcxx::finalize(); 
}
//SNIPPET
