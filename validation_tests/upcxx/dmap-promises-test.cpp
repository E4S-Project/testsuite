#include <iostream>
#include <random>
#include "dmap-promises.hpp"

using namespace std;

int main(int argc, char *argv[])
{
  upcxx::init();
  long N = 10000;
  if (argc > 1) N = std::atol(argv[1]);
  DistrMap dmap;
//SNIPPET
  // create an empty promise, to be used for tracking operations
  upcxx::promise<> prom;
  // insert all key, value pairs into the hash map
  for (long i = 0; i < N; i++) {
    string key = to_string(upcxx::rank_me()) + ":" + to_string(i);
    string val = key;
    // pass the promise to the dmap insert operation
    dmap.insert(key, val, prom);
    // periodically call progress to allow incoming RPCs to be processed
    if (i % 10 == 0) upcxx::progress();
  }
  upcxx::future<> fut = prom.finalize(); // finalize the promise
  fut.wait(); // wait for the operations to complete
//SNIPPET  
  // barrier to ensure all insertions have completed
  upcxx::barrier();
  upcxx::future<> fut_all = upcxx::make_future();
  for (long i = 0; i < N; i++) {
    string key = to_string((upcxx::rank_me() + 1) % upcxx::rank_n()) + ":" + to_string(i);
    // attach callback, which itself returns a future 
    upcxx::future<> fut = dmap.find(key).then(
      // lambda to check the return value
      [key](const string &val) {
        assert(val == key);
      });
    // conjoin the futures
    fut_all = upcxx::when_all(fut_all, fut);
    // periodically call progress to allow incoming RPCs to be processed
    if (i % 10 == 0) upcxx::progress();
  }
  // wait for all the conjoined futures to complete
  fut_all.wait();
  upcxx::barrier();
  if (!upcxx::rank_me()) cout << "SUCCESS" << endl;
  upcxx::finalize();
  return 0;
}


