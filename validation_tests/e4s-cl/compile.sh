#!/bin/bash
. ./setup.sh
set -x
set -e

${TEST_CXX_MPI} ./hello.cpp
