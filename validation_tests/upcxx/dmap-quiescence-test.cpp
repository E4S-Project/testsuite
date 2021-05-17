#include <iostream>
#include <random>
#include <memory>
#include "dmap-ff.hpp"

using namespace std;

// simulate an unpredictable predicate
// test validation relies on this being a pure function
bool should_perform_insert(long i) { return (i%10 != 0); }

int main(int argc, char *argv[])
{
  upcxx::init();
  long N = 10000;
  if (argc > 1) N = std::atol(argv[1]);
  DistrMap dmap;
//SNIPPET  
  // keep track of how many inserts this rank has injected
  long n_inserts_injected = 0;
  // insert all key-value pairs into the hash map
  for (long i = 0; i < N; i++) {
    string key = to_string(upcxx::rank_me()) + ":" + to_string(i);
    string val = key;
    if (should_perform_insert(i)) { // unpredictable condition
      // insert mapping from key to value in our distributed map.
      // insert has no return because it uses rpc_ff.
      dmap.insert(key, val);
      // increment the local count
      n_inserts_injected++;
    }
    // periodically call progress to allow incoming RPCs to be processed
    if (i % 10 == 0) upcxx::progress();
  }

  bool done;
  do { // Loop while not all insert rpc_ff have completed.
    // On each rank, capture the counts of inserts injected and completed
    long local[2] = {n_inserts_injected, dmap.local_size()};
    // Globally count the number of inserts injected and completed by completing
    // an element-wise sum reduction of each of the two counters in the local
    // array, delivering the results in the global array.
    long global[2];
    upcxx::reduce_all(local, global, 2, upcxx::op_fast_add).wait();
    // Test if all inserts have now completed
    assert(global[0] >= global[1]);
    done = (global[0] == global[1]);
  } while (!done);
//SNIPPET  
  upcxx::future<> fut_all = upcxx::make_future();
  for (long i = 0; i < N; i++) {
    string key = to_string((upcxx::rank_me() + 1) % upcxx::rank_n()) + ":" + to_string(i);
    if (should_perform_insert(i)) { 
      // attach callback, which itself returns a future 
      upcxx::future<> fut = dmap.find(key).then(
        // lambda to check the return value
        [key](const string &val) {
          assert(val == key);
        });
      // conjoin the futures
      fut_all = upcxx::when_all(fut_all, fut);
    }
    // periodically call progress to allow incoming RPCs to be processed
    if (i % 10 == 0) upcxx::progress();
  }
  // wait for all the conjoined futures to complete
  fut_all.wait();
  upcxx::barrier(); // wait for finds to complete globally
  if (!upcxx::rank_me()) cout << "SUCCESS" << endl;
  upcxx::finalize();
  return 0;
}


