#!/bin/bash -e

. ./setup.sh



cd build

# Run tests and ensure errors output to standard logs to help with debugging
export CTEST_OUTPUT_ON_FAILURE=1
make test
