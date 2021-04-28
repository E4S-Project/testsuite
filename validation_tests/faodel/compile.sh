#!/bin/bash -e

. ./setup.sh
set -x
cd examples
./compile.sh
RET=$?
cd ..
exit $RET
