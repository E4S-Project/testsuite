#!/bin/bash
. ./setup.sh
set -e
set -x
${TEST_RUN} $LAGHOS_ROOT/bin/laghos -p 1 -dim 2 -rs 3 -tf 0.8 -pa
 
 
