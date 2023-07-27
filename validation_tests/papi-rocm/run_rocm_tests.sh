#!/bin/bash

echo "=== Run rocm test in sampling mode"
ROCP_HSA_INTERCEPT=0 LD_LIBRARY_PATH=$PAPI_ROOT/lib ./single_monitor
ROCP_HSA_INTERCEPT=0 LD_LIBRARY_PATH=$PAPI_ROOT/lib ./multi_monitor --threads=1
ROCP_HSA_INTERCEPT=0 LD_LIBRARY_PATH=$PAPI_ROOT/lib ./multi_monitor --threads=2
ROCP_HSA_INTERCEPT=0 LD_LIBRARY_PATH=$PAPI_ROOT/lib ./overflow

echo "=== Run rocm test in intercept mode"
ROCP_HSA_INTERCEPT=1 LD_LIBRARY_PATH=$PAPI_ROOT/lib ./single_monitor
ROCP_HSA_INTERCEPT=1 LD_LIBRARY_PATH=$PAPI_ROOT/lib ./multi_monitor --threads=1
ROCP_HSA_INTERCEPT=1 LD_LIBRARY_PATH=$PAPI_ROOT/lib ./multi_monitor --threads=2
