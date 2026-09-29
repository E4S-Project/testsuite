#!/bin/bash
. ./setup.sh
set -x
set -e
timeout 3m ${TEST_RUN_SEQ} wannier90.x copper 
cat *.wout
