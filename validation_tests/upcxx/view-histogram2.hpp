#include <upcxx/upcxx.hpp>
#include <cstdint>
#include <map>
#include <string>

//SNIPPET
// Hash a key to its owning rank.
upcxx::intrank_t owner_of(std::string const &key) {
  std::uint64_t h = 0x1234abcd5678cdef;
  for(char c: key)
    h = 63*h + std::uint64_t(c);
  return h % upcxx::rank_n();
}

// This comparison functor orders keys such that they are sorted by 
// owning rank at the expense of rehashing the keys in each invocation.
// A better strategy would be modify the map's key type to compute this
// information once and store it in the map.
struct histogram2_compare {
  bool operator()(std::string const &a, std::string const &b) const {
    using augmented = std::pair<upcxx::intrank_t, std::string const&>;
    return augmented(owner_of(a), a) < augmented(owner_of(b), b);
  }
};

using histogram2 = std::map<std::string, double, histogram2_compare>;

// The target rank's histogram which is updated by incoming rpc's.
histogram2 my_histo2;

// Sending histogram updates by view.
upcxx::future<> send_histo2_byview(histogram2 const &histo) {
  histogram2::const_iterator run_begin = histo.begin();
  
  upcxx::promise<> *all_done = new upcxx::promise<>;
  
  while(run_begin != histo.end()) {
    histogram2::const_iterator run_end = run_begin;
    upcxx::intrank_t owner = owner_of(run_begin->first);
    
    // Compute the end of this run as the beginning of the next run.
    while(run_end != histo.end() && owner_of(run_end->first) == owner)
      ++run_end;
    
    upcxx::rpc(owner,
      upcxx::operation_cx::as_promise(*all_done),
      
      [](upcxx::view<std::pair<const std::string, double>> histo_view) {
        // Traverse key-values directly in network buffer.
        for(auto const &kv: histo_view)
          my_histo2[kv.first] += kv.second;
      },
      // Serialize from a subset of `histo` in-place.
      upcxx::make_view(run_begin, run_end)
    );
    
    run_begin = run_end;
  }
  
  return all_done->finalize().then(
    [=]() { delete all_done; }
  );
}
//SNIPPET
