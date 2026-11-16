#!/bin/bash

set -x

export OMP_NUM_THREADS=8
mrchem --launcher="${TEST_RUN}" water-pbe-mw6
