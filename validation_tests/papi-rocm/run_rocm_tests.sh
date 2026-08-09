#!/bin/bash

HSA_ROOT=$PAPI_ROCM_ROOT
PAPI_ROOT="$(dirname $(dirname `which papi_component_avail`))"

echo "=== Run rocm test in sampling mode"
echo "single monitor" && ROCP_HSA_INTERCEPT=0 LD_LIBRARY_PATH=$PAPI_ROOT/lib:$HSA_ROOT/lib:$LD_LIBRARY_PATH ./single_monitor            && echo ""
echo "multi monitor"  && ROCP_HSA_INTERCEPT=0 LD_LIBRARY_PATH=$PAPI_ROOT/lib:$HSA_ROOT/lib:$LD_LIBRARY_PATH ./multi_monitor --threads=1 && echo ""
echo "multi monitor"  && ROCP_HSA_INTERCEPT=0 LD_LIBRARY_PATH=$PAPI_ROOT/lib:$HSA_ROOT/lib:$LD_LIBRARY_PATH ./multi_monitor --threads=2 && echo ""
echo "overflow"       && ROCP_HSA_INTERCEPT=0 LD_LIBRARY_PATH=$PAPI_ROOT/lib:$HSA_ROOT/lib:$LD_LIBRARY_PATH ./overflow                  && echo ""

echo "=== Run rocm test in intercept mode"
echo "single monitor" && ROCP_HSA_INTERCEPT=1 LD_LIBRARY_PATH=$PAPI_ROOT/lib:$HSA_ROOT/lib:$LD_LIBRARY_PATH ./single_monitor            && echo ""
echo "multi monitor"  && ROCP_HSA_INTERCEPT=1 LD_LIBRARY_PATH=$PAPI_ROOT/lib:$HSA_ROOT/lib:$LD_LIBRARY_PATH ./multi_monitor --threads=1 && echo ""
echo "multi monitor"  && ROCP_HSA_INTERCEPT=1 LD_LIBRARY_PATH=$PAPI_ROOT/lib:$HSA_ROOT/lib:$LD_LIBRARY_PATH ./multi_monitor --threads=2 && echo ""
