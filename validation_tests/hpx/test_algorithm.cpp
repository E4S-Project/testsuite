//  Copyright (c) 2021 Ste||ar Group
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/algorithm.hpp>
#include <hpx/execution.hpp>
#include <hpx/init.hpp>
#include <hpx/program_options.hpp>

#include <check_helper.hpp>

#include <cstdint>
#include <vector>

int hpx_main(hpx::program_options::variables_map &vm) {
  // Smoke test for basic parallel algorithms functionality
  std::uint64_t const n = vm["n"].as<std::uint64_t>();

  std::vector<std::uint64_t> v(n);
  hpx::for_loop(hpx::execution::par, 0, v.size(),
                [&](std::size_t i) { v[i] = i; });
  std::uint64_t const result = hpx::ranges::reduce(hpx::execution::par, v, 0,
                                                   std::plus<std::uint64_t>{});

  std::uint64_t const expected = n * (n - 1) / 2;
  check("reduction", expected, result);

  return hpx::finalize();
}

int main(int argc, char *argv[]) {
  hpx::init_params p;
  hpx::program_options::options_description opts(
      "Usage: test_algorithm [options]");
  opts.add_options()(
      "n", hpx::program_options::value<std::uint64_t>()->default_value(10000),
      "number of elements to use in reduction test");
  p.desc_cmdline = opts;

  return hpx::init(argc, argv, p);
}
