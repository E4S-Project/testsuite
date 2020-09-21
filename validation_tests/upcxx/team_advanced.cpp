#include <upcxx/upcxx.hpp>

using namespace std;

int main() {
  upcxx::init();

  //The following example assumes an even number of processes
  //Gracefully exit if it is not the case
  if (upcxx::rank_n()%2) {
    if (!upcxx::rank_me()) {
      cerr << "This test requires an even number of processes. Test skipped" << endl; 
      cout << "SUCCESS" << endl; 
    }
    upcxx::finalize();
    return 0;
  }

//SNIPPET
  upcxx::team & world_team = upcxx::world();
  int color = upcxx::rank_me() % 2;
  int key = upcxx::rank_me() / 2;
  upcxx::team new_team = world_team.split(color, key);

  upcxx::intrank_t local_rank = new_team.rank_me();
  upcxx::intrank_t local_count = new_team.rank_n();

  upcxx::intrank_t world_rank = new_team[(local_rank+1)%local_count];
  upcxx::intrank_t expected_world_rank = (upcxx::rank_me() + 2) % upcxx::rank_n();
  assert(world_rank == expected_world_rank);

  upcxx::intrank_t other_local_rank = new_team.from_world(world_rank);
  assert(other_local_rank == (local_rank+1)%local_count);
  upcxx::intrank_t non_member_rank = 
    new_team.from_world((upcxx::rank_me()+1)%upcxx::rank_n(),-1);
  assert(non_member_rank == -1);

  new_team.destroy(); // collectively release the sub-team
//SNIPPET

  if (!upcxx::rank_me()) cout << "SUCCESS" << endl;
  upcxx::finalize();
  return 0;
}
