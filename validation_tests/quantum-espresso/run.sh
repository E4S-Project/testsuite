#!/bin/bash
. ./setup.sh
set -e
set -x
export OMP_NUM_THREADS=4
${TEST_RUN}  pw.x -inp pw.scf.silicon.in > pw.scf.silicon.out

grep -e 'total energy' -e estimate pw.scf.silicon.out
 
