#include <iostream>
#include <cstdlib>
#include <random>
#include <upcxx/upcxx.hpp>

using namespace std;

// choose a point at random
int64_t hit()
{
    double x = static_cast<double>(rand()) / RAND_MAX;
    double y = static_cast<double>(rand()) / RAND_MAX;
    if (x*x + y*y <= 1.0) return 1;
    else return 0;
}

// sum the hits to rank 0
// std::int64_t is used to prevent overflows
int64_t reduce_to_rank0(int64_t my_hits)
{
    // wait for a collective reduction that sums all local values
    return upcxx::reduce_all(my_hits, plus<int64_t>()).wait();
}

int main(int argc, char **argv)
{
    upcxx::init();
    // each rank gets its own copy of local variables
    int64_t my_hits = 0;
    // the number of trials to run on each rank
    int my_trials = 100000;
    // each rank gets its own local copies of input arguments
    if (argc == 2) my_trials = atoi(argv[1]);
    // initialize the random number generator differently for each rank
    srand(upcxx::rank_me());
    // do the computation
    for (int i = 0; i < my_trials; i++) {
        my_hits += hit();
    }
    // sum the hits and print out the final result
    int64_t hits = reduce_to_rank0(my_hits);
    // only rank 0 prints the result
    if (upcxx::rank_me() == 0) {
        // the total number of trials over all ranks
        int64_t trials = upcxx::rank_n() * my_trials;
        cout << "pi estimate: " << 4.0 * hits / trials << ", "
             << "rank 0 alone: " << 4.0 * my_hits / my_trials << endl; 
    }
    upcxx::finalize();
    return 0;
}
