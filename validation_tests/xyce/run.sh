#!/bin/bash
. ./setup.sh
# export MV2_HOMOGENEOUS_CLUSTER=1
# export MV2_SUPPRESS_JOB_STARTUP_PERFORMANCE_WARNING=1
set -x
set -e
${TEST_RUN} Xyce ./rc_simple_xyce.cir
 
 
