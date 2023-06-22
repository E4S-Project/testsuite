//  Copyright (c) 2021-2023 Ste||ar Group
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/config.hpp>
#if !defined(HPX_COMPUTE_DEVICE_CODE)
#include <hpx/future.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/runtime.hpp>
#include <hpx/iostream.hpp>
#include <hpx/wrap_main.hpp>

#include <ostream>
#include <vector>

void hello_world() {
  hpx::cout << "Hello world from locality " << hpx::get_locality_id()
            << std::endl;
}

HPX_PLAIN_ACTION(hello_world, hello_world_action);

int main() {
  std::vector<hpx::id_type> localities = hpx::find_all_localities();
  std::vector<hpx::future<void>> futures;
  futures.reserve(localities.size());

  for (hpx::id_type const &locality : localities) {
    futures.push_back(hpx::async<hello_world_action>(locality));
  }

  hpx::when_all(futures).get();
  return 0;
}
#endif
