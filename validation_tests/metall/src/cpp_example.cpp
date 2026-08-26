// Copyright 2021 Lawrence Livermore National Security, LLC and other Metall
// Project Developers. See the top-level COPYRIGHT file for details.
//
// SPDX-License-Identifier: (Apache-2.0 OR MIT)

#include <metall/metall.hpp>
#include <cstdlib>

int main() {
  const char* env_dir = std::getenv("METALL_TEST_DIR");
  const char* target_dir = env_dir ? env_dir : "/tmp/dir";

  metall::manager manager(metall::create_only, target_dir);
  return 0;
}
