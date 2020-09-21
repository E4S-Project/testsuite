#include <upcxx/upcxx.hpp>

using namespace std;

int main() {
  upcxx::init();

//SNIPPET
  upcxx::team & world_team = upcxx::world();  
  int color = upcxx::rank_me() % 2;
  int key = upcxx::rank_me() / 2;
  upcxx::team new_team = world_team.split(color, key);
//SNIPPET
  
  if (!upcxx::rank_me()) cout << "SUCCESS" << endl;
  upcxx::finalize();
  return 0;
}
