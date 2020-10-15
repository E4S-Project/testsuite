#!/bin/bash -ex

. ./setup.sh

cd examples
./compile.sh
RET=$?
cd ..
exit $RET
