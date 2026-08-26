// Copyright 2021 Lawrence Livermore National Security, LLC and other Metall
// Project Developers. See the top-level COPYRIGHT file for details.
//
// SPDX-License-Identifier: (Apache-2.0 OR MIT)

#include <metall/c_api/metall.h>
#include <stdlib.h>

int main(void) {
  const char* env_dir = getenv("METALL_TEST_DIR");
  const char* target_dir = env_dir ? env_dir : "/tmp/dir";

  metall_open(METALL_CREATE_ONLY, target_dir);
  metall_close();

  return 0;
}
