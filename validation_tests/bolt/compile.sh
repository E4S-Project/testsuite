#!/bin/bash
. ./setup.sh
set -x
eval $TEST_CC sample_nested.c -L${BOLT_ROOT}/lib -lomp
