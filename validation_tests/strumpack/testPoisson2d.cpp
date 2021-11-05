/*
 * STRUMPACK -- STRUctured Matrices PACKage, Copyright (c) 2014, The
 * Regents of the University of California, through Lawrence Berkeley
 * National Laboratory (subject to receipt of any required approvals
 * from the U.S. Dept. of Energy).  All rights reserved.
 *
 * If you have questions about your rights to use or distribute this
 * software, please contact Berkeley Lab's Technology Transfer
 * Department at TTD@lbl.gov.
 *
 * NOTICE. This software is owned by the U.S. Department of Energy. As
 * such, the U.S. Government has been granted for itself and others
 * acting on its behalf a paid-up, nonexclusive, irrevocable,
 * worldwide license in the Software to reproduce, prepare derivative
 * works, and perform publicly and display publicly.  Beginning five
 * (5) years after the date permission to assert copyright is obtained
 * from the U.S. Department of Energy, and subject to any subsequent
 * five (5) year renewals, the U.S. Government is granted for itself
 * and others acting on its behalf a paid-up, nonexclusive,
 * irrevocable, worldwide license in the Software to reproduce,
 * prepare derivative works, distribute copies to the public, perform
 * publicly and display publicly, and to permit others to do so.
 *
 * Developers: Pieter Ghysels, Francois-Henry Rouet, Xiaoye S. Li.
 *             (Lawrence Berkeley National Lab, Computational Research
 *             Division).
 *
 */
#include <iostream>
#include "StrumpackSparseSolver.hpp"

#define ERROR_TOLERANCE 1e2

typedef double scalar;
typedef double real;
typedef int integer;

using namespace strumpack;

int main(int argc, char* argv[]) {
  int n = 30;
  int nrhs = 10;
  if (argc > 1) n = atoi(argv[1]); // get grid size
  else std::cout << "# please provide grid size" << std::endl;
  // get number of right-hand sides
  if (argc > 2) nrhs = std::max(1, atoi(argv[2]));
  std::cout << "solving 2D " << n << "x" << n << " Poisson problem"
            << " with " << nrhs << " right hand sides" << std::endl;

  StrumpackSparseSolver<scalar,integer> spss;
  spss.options().set_matching(MatchingJob::NONE);
  spss.options().set_reordering_method(ReorderingStrategy::GEOMETRIC);
  spss.options().set_from_command_line(argc, argv);

  integer N = n * n;
  integer nnz = 5 * N - 4 * n;
  CSRMatrix<scalar,integer> A(N, nnz);
  integer* ptr = A.ptr();
  integer* ind = A.ind();
  scalar* val = A.val();

  nnz = 0;
  ptr[0] = 0;
  for (integer row=0; row<n; row++) {
    for (integer col=0; col<n; col++) {
      integer i = col+n*row;
      val[nnz] = 4.0;
      ind[nnz++] = i;
      if (col > 0)  { val[nnz] = -1.0; ind[nnz++] = i-1; } // left
      if (col < n-1){ val[nnz] = -1.0; ind[nnz++] = i+1; } // right
      if (row > 0)  { val[nnz] = -1.0; ind[nnz++] = i-n; } // up
      if (row < n-1){ val[nnz] = -1.0; ind[nnz++] = i+n; } // down
      ptr[i+1] = nnz;
    }
  }
  A.set_symm_sparse();

  DenseMatrix<scalar> b(N, nrhs), x(N, nrhs), x_exact(N, nrhs);
  x_exact.random();
  A.spmv(x_exact, b);

  spss.set_csr_matrix(N, ptr, ind, val, true);
  if (spss.reorder(n, n) != ReturnCode::SUCCESS) {
    std::cerr << "Problem with reordering of the matrix." << std::endl;
    return 1;
  }
  if (spss.factor() != ReturnCode::SUCCESS) {
    std::cerr << "Problem during factorization of the matrix." << std::endl;
    return 1;
  }
  if (spss.solve(b, x) != ReturnCode::SUCCESS) {
    std::cerr << "Problem during solve phase." << std::endl;
    return 1;
  }

  auto scaled_res = A.max_scaled_residual(x, b);
  std::cout << "# COMPONENTWISE SCALED RESIDUAL = "
            << scaled_res << std::endl;
  x.scaled_add(-1., x_exact);
  std::cout << "# relative error = ||x-x_exact||_F/||x_exact||_F = "
            << x.normF() / x_exact.normF() << std::endl;
  if (scaled_res > ERROR_TOLERANCE*spss.options().rel_tol()) {
    std::cerr << "Solve did not reach required accuracy." << std::endl;
    return 1;
  }
  return 0;
}
