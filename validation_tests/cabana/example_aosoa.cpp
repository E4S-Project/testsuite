/****************************************************************************
 * Copyright (c) 2018-2021 by the Cabana authors                            *
 * All rights reserved.                                                     *
 *                                                                          *
 * This file is part of the Cabana library. Cabana is distributed under a   *
 * BSD 3-clause license. For the licensing terms see the LICENSE file in    *
 * the top-level directory.                                                 *
 *                                                                          *
 * SPDX-License-Identifier: BSD-3-Clause                                    *
 ****************************************************************************/

#include <Cabana_Core.hpp>

#include <Kokkos_Core.hpp>

#include <array>
#include <iostream>
#include <numeric>

int main(int argc, char *argv[]) {
  Kokkos::ScopeGuard guard(argc, argv);

  using DataTypes = Cabana::MemberTypes<double, double, float[3][3]>;
  const int VectorLength = 16;

#ifdef Kokkos_ENABLE_CUDA
  using MemorySpace = Kokkos::CudaSpace;
  using ExecutionSpace = Kokkos::Cuda;
#else
  using MemorySpace = Kokkos::HostSpace;
  using ExecutionSpace = Kokkos::Serial;
#endif

  // Create the data.
  int num_p = 25;
  Cabana::AoSoA<DataTypes, MemorySpace, VectorLength> aosoa("aosoa", num_p);

  // Fill the data.
  auto slice_0 = Cabana::slice<0>(aosoa);
  auto slice_1 = Cabana::slice<1>(aosoa);
  for (int i = 0; i < num_p; ++i) {
    slice_0(i) = 0.0;
    slice_1(i) = 1.0;
  }

  // Define the parallel kernel.
  auto vector_kernel = KOKKOS_LAMBDA(const int s, const int a) {
    slice_0.access(s, a) += slice_1.access(s, a);
  };
  Cabana::SimdPolicy<VectorLength, ExecutionSpace> simd_policy(0, num_p);

  // Update the data in parallel.
  Cabana::simd_parallel_for(simd_policy, vector_kernel, "vector_op");

  // Copy to host.
  auto host_aosoa =
      Cabana::create_mirror_view_and_copy(Kokkos::HostSpace(), aosoa);
  auto host_slice_0 = Cabana::slice<0>(host_aosoa);

  // Check total.
  double sum = 0;
  for (std::size_t i = 0; i < num_p; ++i) {
    sum += host_slice_0(i);
  }
  return (sum == num_p) ? 0 : -1;
}
