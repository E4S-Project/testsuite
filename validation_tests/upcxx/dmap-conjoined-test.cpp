#include <iostream>
#include <random>
#include "dmap.hpp"

using namespace std;

int main(int argc, char *argv[])
{
  upcxx::init();
  const long N = 100000;
  DistrMap dmap;
  string key = to_string(upcxx::rank_me()) + ":" + to_string(0);
  string val = key;
  // insert all key, value pairs into the hash map, wait for operation to complete
  for (long i = 0; i < N; i++) {
    upcxx::future<> fut = dmap.insert(key, val);
    // compute new key while waiting for RPC to complete
    if (i < N - 1) {
      key = to_string(upcxx::rank_me()) + ":" + to_string(i + 1);
      val = key;
    }
    // wait for operation to complete before next insert
    fut.wait();
  }
  // barrier to ensure all insertions have completed
  upcxx::barrier();
//SNIPPET
  // the start of the conjoined future
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
//SNIPPET
  upcxx::barrier(); // wait for finds to complete globally
  if (!upcxx::rank_me()) cout << "SUCCESS" << endl;
  upcxx::finalize();
  return 0;
}


