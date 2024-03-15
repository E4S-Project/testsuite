#!/bin/bash

. ../../setup.sh
spackLoadUniqueNoR dyninst
spackLoadUniqueNoX cmake
spackLoadUniqueNoX boost
spackLoadUniqueNoX elfutils
spackLoadUniqueNoX intel-tbb

mkdir -p xmas_tree/build
