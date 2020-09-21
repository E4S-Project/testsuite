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
  // distributed object to keep track of number of inserts expected at this process
  upcxx::dist_object<long> n_inserts = 0;
  // keep track of how many inserts have been made to each target process
  std::unique_ptr<long[]> inserts_per_rank(new long[upcxx::rank_n()]());
  // insert all key-value pairs into the hash map
  for (long i = 0; i < N; i++) {
    string key = to_string(upcxx::rank_me()) + ":" + to_string(i);
    string val = key;
    // insert has no return because it uses rpc_ff
    dmap.insert(key, val);
    // increment the count for the target process
    inserts_per_rank[dmap.get_target_rank(key)]++;
    // periodically call progress to allow incoming RPCs to be processed
    if (i % 10 == 0) upcxx::progress();
  }
  // update all remote processes with the expected count
  for (long i = 0; i < upcxx::rank_n(); i++) {
    if (inserts_per_rank[i]) {
      // use rpc to update the remote process's expected count of inserts
      upcxx::rpc(i,
                 [](upcxx::dist_object<long> &e_inserts, long count) {
                   *e_inserts += count;
                 }, n_inserts, inserts_per_rank[i]).wait();
    }
  }
  // wait until all threads have updated insert counts
  upcxx::barrier();
  long expected_inserts = *n_inserts;
  // wait until we have received all the expected updates, spinning on progress
  while (dmap.local_size() < expected_inserts) upcxx::progress();
//SNIPPET  
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


