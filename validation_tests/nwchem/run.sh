#!/bin/bash
. ./setup.sh
set -e
set -x
${TEST_RUN} nwchem h2o
