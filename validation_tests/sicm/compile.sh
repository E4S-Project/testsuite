#!/usr/bin/env bash

set -e

. ./setup.sh

${TEST_CC} -std=c99 walk_devices.c -o walk_devices -I${SICM_ROOT}/include -L${SICM_ROOT}/lib -lsicm
