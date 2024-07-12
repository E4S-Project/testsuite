#!/bin/bash
. ./setup.sh
set -x
set -e

${TEST_RUN} ./xdscaex  
${TEST_RUN} ./xzscaex  
${TEST_RUN} ./xcscaex        
${TEST_RUN} ./xsscaex  
