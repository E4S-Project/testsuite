#!/bin/bash
. ./setup.sh
set -e
set -x
cd ./example1
${TEST_RUN} ./nekbone 
 
