#include <iostream>
#include <random>
#include "dmap.hpp"

using namespace std;

int main(int argc, char *argv[])
{
  upcxx::init();
  long N = 10000;
  if (argc > 1) N = std::atol(argv[1]);
  DistrMap dmap;
//SNIPPET
  // initialize key and value for first insertion
  string key = to_string(upcxx::rank_me()) + ":" + to_string(0);
  string val = key;
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
//SNIPPET
  // barrier to ensure all insertions have completed
  upcxx::barrier();
  for (long i = 0; i < N; i++) {
    string key = to_string((upcxx::rank_me() + 1) % upcxx::rank_n()) + ":" + to_string(i);
    // attach callback, which itself returns a future 
    upcxx::future<> fut = dmap.find(key).then(
      // lambda to check the return value
      [key](const string &val) {
        assert(val == key);
      });
    // wait for future and its callback to complete
    fut.wait();
  }
  upcxx::barrier(); // wait for finds to complete globally
  if (!upcxx::rank_me()) cout << "SUCCESS" << endl;
  upcxx::finalize();
  return 0;
}


