#!/bin/bash
. ./setup.sh
set -e
set -x
CALI_CONFIG=runtime-report,rocm-activity-report ./HelloWorld  #rocm.gputime
 
