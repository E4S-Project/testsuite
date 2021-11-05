//  Copyright (c) 2021 Ste||ar Group
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <sstream>
#include <stdexcept>
#include <string>

template <typename T, typename U>
void check(std::string const &test_name, T &&expected, U &&result) {
  if (result != expected) {
    std::ostringstream s;
    s << "HPX \"" << test_name << "\" test failed. Expected " << expected
      << ", got " << result;
    throw std::runtime_error(s.str());
  }
}
