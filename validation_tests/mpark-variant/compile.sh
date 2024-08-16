#!/bin/bash
. ./setup.sh
set -x
set -e

${TEST_CXX}  -I$MPARK_VARIANT_ROOT/include ./variant.cpp  
