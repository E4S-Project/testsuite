#!/bin/bash
. ./setup.sh
set -e
set -x
rm -f ./profile.*
tau_exec -T SERIAL -L0  ./ze_gemm/build/ze_gemm
pprof
