#include "view-histogram2.hpp"
#include <iostream>

int main() {
  upcxx::init();
  
  histogram2 part;
  
  for(int i=0; i < 1000; i++) {
    std::string key = "key:" + std::to_string((upcxx::rank_me() + i) % 100);
    part[key] += 1;
  }
  
  send_histo2_byview(part).wait();
  
  upcxx::barrier();
  
  int sum = 0;
  for(auto const &kv: my_histo2)
    sum += int(kv.second);
  
  sum = upcxx::reduce_all(sum, upcxx::op_fast_add).wait();
  
  assert(sum == 1000*upcxx::rank_n());
  
  if(upcxx::rank_me() == 0)
    std::cout << "SUCCESS\n";
  
  upcxx::finalize();
  return 0;
}
