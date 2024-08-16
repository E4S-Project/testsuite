#!/bin/bash
. ./setup.sh
set -e
set -x
e4s-cl launch  ${TEST_RUN} ./a.out
 
