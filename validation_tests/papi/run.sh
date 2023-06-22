#!/bin/bash
. ./setup.sh
set -e
set -x
./simple
./threads

#${TEST_RUN} ./lu.W.4 
 
