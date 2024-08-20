#!/bin/bash
. ./setup.sh
set -e
set -x

mkdir test
${TEST_RUN} dcp ../* ./test
${TEST_RUN} dwalk -p ./test
${TEST_RUN} drm ./test
