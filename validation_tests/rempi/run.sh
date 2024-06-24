#!/bin/bash
. ./setup.sh
set -x
set -e
rempi_record ${TEST_RUN} ./example1
rempi_replay ${TEST_RUN} ./example1

rempi_record REMPI_DIR=/tmp ${TEST_RUN} ./example1
rempi_replay REMPI_DIR=/tmp ${TEST_RUN} ./example1

rempi_record REMPI_DIR=/tmp REMPI_GZIP=1 ${TEST_RUN} ./example1
rempi_replay REMPI_DIR=/tmp REMPI_GZIP=1 ${TEST_RUN} ./example1

rempi_record ${TEST_RUN} ./example1
#rempi_replay totalview -args srun -n 4 ./example1

