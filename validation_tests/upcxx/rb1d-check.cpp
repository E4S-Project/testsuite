//SNIPPET
bool check_convergence(double *u, long n_local, const double EXPECTED_VAL,
                       const double EPSILON, long stepi)
{
  double err = 0;
  for (long i = 1; i < n_local - 1; i++)
    err = max(err, fabs(EXPECTED_VAL - u[i]));
  // upcxx collective to get max error over all processes
  double max_err = upcxx::reduce_all(err, upcxx::op_fast_max).wait();
  // check for convergence
  if (max_err / EXPECTED_VAL <= EPSILON) {
    if (!upcxx::rank_me())
      cout << "Converged at " << stepi <<", err " << max_err << endl;
    return true;
  }
  return false;
}
//SNIPPET
