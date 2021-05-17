#include <upcxx/upcxx.hpp>
#include <array>
#include <vector>

//SNIPPET
double my_matrix[1000][1000] = {/*0...*/}; // Rank's local matrix.
constexpr int worker_n = 8; // Number of worker threads/personas.

// Each persona has a dedicated thread spinning on its progress engine.
upcxx::persona workers[worker_n];

struct element {
  int row, col;
  double value;
};

upcxx::future<> update_remote_matrix(upcxx::intrank_t rank,
                                     element const *elts, int elt_n) {
  return upcxx::rpc(rank,
    [](upcxx::view<element> const &elts_in_rpc) {
      upcxx::future<> all_done = upcxx::make_future();
      for(int w=0; w < worker_n; w++) {
        // Launch task on respective worker.
        auto task_done = workers[w].lpc(
          [w, elts_in_rpc]() {
            // Sum subset of elements into `my_matrix` according to a
            // round-robin mapping of matrix rows to workers.
            for(element const &elt: elts_in_rpc) {
              if(w == elt.row % worker_n)
                my_matrix[elt.row][elt.col] += elt.value;
            }
          }
        );
        // Conjoin task completion into `all_done`.
        all_done = upcxx::when_all(all_done, task_done);
      }
      // Returned future has a dependency on each task lpc so the network
      // buffer (thus `elts_in_rpc` view) will remain valid until all tasks
      // have completed.
      return all_done;
    },
    upcxx::make_view(elts, elts + elt_n)
  );
}
//SNIPPET
