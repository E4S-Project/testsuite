#!/bin/bash
. ./setup.sh
set -x
${TEST_RUN} ./build/downstream-app  
${TEST_RUN} ./build/maps-and-vectors  
${TEST_RUN} ./build/power-method
 
