#!/bin/bash -e
. ./setup.sh
set -x
timeout 5m ${VISIT_ROOT}/bin/visit -cli -nowin -s test1.py
