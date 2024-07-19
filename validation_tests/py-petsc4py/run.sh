#!/bin/bash
. ./setup.sh
set -e
set -x
${TEST_RUN} python ./poisson2d.py -view_sol -view_mat 
 
