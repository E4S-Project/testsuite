#include <iostream>
#include <random>
#include <memory>
#include "dmap-ff.hpp"

using namespace std;

int main(int argc, char *argv[])
{
  upcxx::init();
  const long N = 100000;
  DistrMap dmap;
//SNIPPET
  // keep track of how many inserts have been made to each target process
  std::unique_ptr<int64_t[]> inserts_per_rank(new int64_t[upcxx::rank_n()]());
  // insert all key-value pairs into the hash map
  for (long i = 0; i < N; i++) {
    string key = to_string(upcxx::rank_me()) + ":" + to_string(i);
    string val = key;
    dmap.insert(key, val);
    inserts_per_rank[dmap.get_target_rank(key)]++;
     // periodically call progress to allow incoming RPCs to be processed
    if (i % 10 == 0) upcxx::progress();
 }
  // setup atomic domain with only the operations needed
  upcxx::atomic_domain<int64_t> ad({upcxx::atomic_op::load, upcxx::atomic_op::add});
  // distributed object to keep track of number of inserts expected at every process
  upcxx::dist_object<upcxx::global_ptr<int64_t> > n_inserts(upcxx::new_<int64_t>(0));
  // get pointers for all other processes and use atomics to update remote counters
  for (long i = 0; i < upcxx::rank_n(); i++) {
    if (inserts_per_rank[i]) {
      upcxx::global_ptr<int64_t> remote_n_inserts = n_inserts.fetch(i).wait();
      // use atomics to increment the remote process's expected count of inserts
      ad.add(remote_n_inserts, inserts_per_rank[i], memory_order_relaxed).wait();
    }
  }
  upcxx::barrier();
  // Note: once a memory location is accessed with atomics, it should only be
  // subsequently accessed using atomics to prevent unexpected results
  int64_t expected_inserts = ad.load(*n_inserts, memory_order_relaxed).wait();
  // wait until we have received all the expected updates, spinning on progress
  while (dmap.local_size() < expected_inserts) upcxx::progress();
//SNIPPET  
  // now try to fetch keys inserted by neighbor
  upcxx::future<> fut_all = upcxx::make_future();
  for (long i = 0; i < N; i++) {
    string key = to_string((upcxx::rank_me() + 1) % upcxx::rank_n()) + ":" + to_string(i);
    // attach callback, which itself returns a future 
    upcxx::future<> fut = dmap.find(key).then(
      // lambda to check the return value
      [key](string val) {
        assert(val == key);
      });
    // conjoin the futures
    fut_all = upcxx::when_all(fut_all, fut);
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


