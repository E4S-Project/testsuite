#!/bin/bash
. ./setup.sh
set -e
set -x
${TEST_RUN} ./ninja_test_pingpong 
 
