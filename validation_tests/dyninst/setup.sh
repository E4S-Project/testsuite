#!/bin/bash

. ../../setup.sh
spackLoadUniqueNoR dyninst
spackLoadUniqueNoR cmake
spackLoadUniqueNoR boost
spackLoadUniqueNoR elfutils
spackLoadUniqueNoR intel-tbb

mkdir -p xmas_tree/build
