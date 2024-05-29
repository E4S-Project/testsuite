#!/bin/bash
. ./setup.sh
set -x
set -e
${TEST_RUN} ./poisson/build/app/poisson 100 100 -m 10000 
ls *.dat
