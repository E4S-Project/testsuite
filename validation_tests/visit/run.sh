#!/bin/bash -e
. ./setup.sh
set -x
${VISIT_ROOT}/bin/visit -cli -nowin -s test1.py
