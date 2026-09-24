#!/bin/bash -e
. ./setup.sh
cd examples/*/
ctest --output-on-failure
