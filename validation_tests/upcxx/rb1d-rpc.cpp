#include <iostream>
#include <random>
#include <cassert>
#include <upcxx/upcxx.hpp>

using namespace std;

#include "rb1d-check.cpp"

// An alternative version of rb1d-rpcinit.cpp, using RPC in place of RMA.
// This version is notably free of barrier synchronization (aside from the convergeance check).

//SNIPPET
int main(int argc, char **argv) {
  upcxx::init();
  // initialize parameters - simple test case
  const long N = 1024;
  const long MAX_ITER = N * N * 2; 
  const double EPSILON = 0.1;
  const int MAX_VAL = 100;
  const double EXPECTED_VAL = MAX_VAL / 2;
  // get the bounds for the local panel, assuming num procs divides N into an even block size
  long block = N / upcxx::rank_n();
  assert(block % 2 == 0); assert(N == block * upcxx::rank_n());
  // plus two for ghost cells
  long n_local = block + 2;
  // set up the local array, in private memory
  static double *u = new double[n_local];
  // init to uniform pseudo-random distribution, independent of job size
  mt19937_64 rgen(1);  rgen.discard(upcxx::rank_me() * block);
  for (long i = 1; i < n_local - 1; i++)  
    u[i] = 0.5 + rgen() % MAX_VAL;
  // fetch the left and right pointers for the ghost cells
  int l_nbr = (upcxx::rank_me() + upcxx::rank_n() - 1) % upcxx::rank_n();
  int r_nbr = (upcxx::rank_me() + 1) % upcxx::rank_n();
  static long flag[2] = { -1, -1 };
  // iteratively solve
  for (long stepi = 0; stepi < MAX_ITER; stepi++) {
    // alternate between red and black
    int phase = stepi % 2;
    auto push = [stepi,phase](long idx, double val) { u[idx] = val; flag[phase] = stepi; };
    // push the value for the ghost cell
    if (!phase) upcxx::rpc_ff(r_nbr, push, 0, u[block]);
    else        upcxx::rpc_ff(l_nbr, push, block+1, u[1]);
    while (flag[phase] != stepi) upcxx::progress();
    // compute updates and error
    for (long i = phase + 1; i < n_local - 1; i += 2) 
      u[i] = (u[i - 1] + u[i + 1]) / 2.0;
    // periodically check convergence
    if (stepi % 10 == 0) {
      if (check_convergence(u, n_local, EXPECTED_VAL, EPSILON, stepi))
        break;
    }
  }
  upcxx::finalize();
  return 0;
}
//SNIPPET
