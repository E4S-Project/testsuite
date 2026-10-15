#!/bin/bash
. ./setup.sh
set -x
set -e
rempi_record ${TEST_RUN} ./example1
rempi_replay ${TEST_RUN} ./example1

REMPI_TMPROOT=$(mktemp -d "${TMPDIR:-/tmp}/rempi-test.XXXXXXXX")
trap 'rm -rf "$REMPI_TMPROOT"' EXIT

mkdir "$REMPI_TMPROOT/plain"
rempi_record REMPI_DIR="$REMPI_TMPROOT/plain" ${TEST_RUN} ./example1
rempi_replay REMPI_DIR="$REMPI_TMPROOT/plain" ${TEST_RUN} ./example1

mkdir "$REMPI_TMPROOT/gzip"
rempi_record REMPI_DIR="$REMPI_TMPROOT/gzip" REMPI_GZIP=1 ${TEST_RUN} ./example1
rempi_replay REMPI_DIR="$REMPI_TMPROOT/gzip" REMPI_GZIP=1 ${TEST_RUN} ./example1

rempi_record ${TEST_RUN} ./example1
#rempi_replay totalview -args srun -n 4 ./example1

