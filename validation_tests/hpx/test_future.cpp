//  Copyright (c) 2021 Ste||ar Group
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/execution.hpp>
#include <hpx/future.hpp>
#include <hpx/init.hpp>
#include <hpx/program_options.hpp>
#include <hpx/thread.hpp>

#include <check_helper.hpp>

#include <chrono>
#include <cstdint>

int hpx_main(hpx::program_options::variables_map &vm) {
  // Smoke test for basic futures functionality
  std::uint64_t const n = vm["n"].as<std::uint64_t>();

  auto slow_function = [n]() {
    hpx::this_thread::sleep_for(std::chrono::milliseconds(500));
    return 2 * n;
  };
  auto join_function = [](std::uint64_t x, std::uint64_t y) { return x + y; };
  hpx::future<std::uint64_t> result_future =
      hpx::dataflow(hpx::unwrapping(join_function), hpx::async(slow_function),
                    hpx::make_ready_future(1));
  std::uint64_t const result = result_future.get();

  std::uint64_t const expected = 2 * n + 1;
  check("future", expected, result);

  return hpx::finalize();
}

int main(int argc, char *argv[]) {
  hpx::init_params p;
  hpx::program_options::options_description opts("Usage: test [options]");
  opts.add_options()(
      "n", hpx::program_options::value<std::uint64_t>()->default_value(42),
      "input value for future test");
  p.desc_cmdline = opts;

  return hpx::init(argc, argv, p);
}
