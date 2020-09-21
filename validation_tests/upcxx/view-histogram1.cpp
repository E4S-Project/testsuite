#include "view-histogram1.hpp"
#include <iostream>
#include <cassert>

int main() {
  upcxx::init();
  
  histogram1 part;
  
  for(int i=0; i < 1000; i++) {
    std::string key = "key:" + std::to_string((upcxx::rank_me() + i) % 100);
    part[key] += 1;
  }
  
  upcxx::when_all(
      send_histo1_byval(part),
      send_histo1_byview(part)
    ).wait();
  
  upcxx::barrier();
  
  int sum = 0;
  for(auto const &kv: my_histo1)
    sum += int(kv.second);
  
  sum = upcxx::reduce_all(sum, upcxx::op_fast_add).wait();
  
  assert(sum == 2*1000*upcxx::rank_n());
  
  if(upcxx::rank_me() == 0)
    std::cout << "SUCCESS\n";
  
  upcxx::finalize();
  return 0;
}
