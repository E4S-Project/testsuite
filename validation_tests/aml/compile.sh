#!/bin/bash

. ./setup.sh
set -x
$TEST_CC -o hello-world hello-world.c -I${NUMACTL_ROOT}/include -I${AML_ROOT}/include -L${AML_ROOT}/lib  -laml -lexcit -pthread
