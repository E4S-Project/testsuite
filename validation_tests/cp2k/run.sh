#!/bin/bash -e
. ./setup.sh
set -x
export OMP_NUM_THREADS=1
${TEST_RUN} cp2k.psmp -i h2o-gpw-pbe.inp -o h2o-gpw-pbe.out
grep -e "PROGRAM ENDED" -e "ENERGY| Total FORCE_EVAL" h2o-gpw-pbe.out
