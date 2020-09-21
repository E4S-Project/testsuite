#include <upcxx/upcxx.hpp>
#include <cstdint>
#include <string>
#include <unordered_map>

//SNIPPET
// Hash a key to its owning rank.
upcxx::intrank_t owner_of(std::string const &key) {
  std::uint64_t h = 0x1234abcd5678cdef;
  for(char c: key)
    h = 63*h + std::uint64_t(c);
  return h % upcxx::rank_n();
}

using histogram1 = std::unordered_map<std::string, double>;

// The target rank's histogram which is updated by incoming rpc's.
histogram1 my_histo1;

// Sending histogram updates by value.
upcxx::future<> send_histo1_byval(histogram1 const &histo) {
  std::unordered_map<upcxx::intrank_t, histogram1> clusters;
  
  // Cluster histogram elements by owning rank.
  for(auto const &kv: histo)
    clusters[owner_of(kv.first)].insert(kv);
  
  upcxx::promise<> *all_done = new upcxx::promise<>;
  
  // Send per-owner histogram clusters.
  for(auto const &cluster: clusters) {
    upcxx::rpc(cluster.first,
      upcxx::operation_cx::as_promise(*all_done),
      
      [](histogram1 const &histo) {
        // Pain point: UPC++ already traversed the key-values once to build the
        // `histo` container. Now we traverse again within the RPC body.
        
        for(auto const &kv: histo)
          my_histo1[kv.first] += kv.second;
        
        // Pain point: UPC++ will now destroy the container.
      },
      cluster.second
    );
  }
  
  return all_done->finalize().then(
    [=]() { delete all_done; }
  );
}

// Sending histogram updates by view.
upcxx::future<> send_histo1_byview(histogram1 const &histo) {
  std::unordered_map<upcxx::intrank_t, histogram1> clusters;
  
  // Cluster histogram elements by owning rank.
  for(auto const &kv: histo)
    clusters[owner_of(kv.first)].insert(kv);
  
  upcxx::promise<> *all_done = new upcxx::promise<>;
  
  // Send per-owner histogram clusters.
  for(auto const &cluster: clusters) {
    upcxx::rpc(cluster.first,
      upcxx::operation_cx::as_promise(*all_done),
      
      [](upcxx::view<std::pair<const std::string, double>> histo_view) {
        // Pain point from `send_histo1_byval`: Eliminated.
        
        // Traverse key-values directly in network buffer.
        for(auto const &kv: histo_view)
          my_histo1[kv.first] += kv.second;
        
        // Pain point from `send_histo1_byval`: Eliminated.
      },
      upcxx::make_view(cluster.second) // build view from container's begin()/end()
    );
  }
  
  return all_done->finalize().then(
    [=]() { delete all_done; }
  );
}
//SNIPPET
