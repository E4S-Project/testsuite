//  Copyright (c) 2021 Ste||ar Group
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/algorithm.hpp>
#include <hpx/config.hpp>
#include <hpx/execution.hpp>
#include <hpx/include/compute.hpp>
#include <hpx/init.hpp>

#include <check_helper.hpp>

#include <cstdint>
#include <iostream>
#include <sstream>
#include <vector>

#if defined(HPX_HAVE_CUDA) || defined(HPX_HAVE_HIP)
int hpx_main(hpx::program_options::variables_map &vm) {
  // Smoke test for basic CUDA/HIP functionality
  std::uint64_t const n = vm["n"].as<std::uint64_t>();

  hpx::cuda::experimental::target target;
  hpx::cuda::experimental::allocator<std::uint64_t> alloc(target);
  hpx::cuda::experimental::default_executor exec(target);

  std::vector<std::uint64_t> A_host(n);
  hpx::compute::vector<std::uint64_t, decltype(alloc)> A_device(n, alloc);

  hpx::for_loop(hpx::execution::par, 0, n,
                [&](std::uint64_t i) { A_host[i] = i; });
  hpx::copy(hpx::execution::par, A_host.begin(), A_host.end(),
            A_device.begin());
  hpx::ranges::for_each(hpx::execution::par.on(exec), A_device,
                        [] HPX_HOST_DEVICE(std::uint64_t & i) { i += 5; });
  hpx::copy(hpx::execution::par, A_device.begin(), A_device.end(),
            A_host.begin());
  hpx::for_loop(hpx::execution::par, 0, n, [&](std::uint64_t i) {
    std::ostringstream s;
    s << "gpu (element " << i << "/" << n << ")";
    check(s.str(), i + 5, A_host[i]);
  });

  return hpx::finalize();
}

int main(int argc, char *argv[]) {
  hpx::init_params p;
  hpx::program_options::options_description opts("Usage: test_gpu [options]");
  opts.add_options()(
      "n", hpx::program_options::value<std::uint64_t>()->default_value(10000),
      "number of elements to use in GPU test");
  p.desc_cmdline = opts;

  return hpx::init(argc, argv, p);
}
#else
int main() {
  std::cerr << "skipping HPX GPU test, CUDA/HIP support not available\n";
}
#endif
