#!/bin/bash
. ./setup.sh
set -x
set -e

${TEST_CC_MPI} -DHAVE_CONFIG_H -I.      -g -O2 -MT ninja_test_util.o -MD -MP  -c -o ninja_test_util.o ninja_test_util.c
${TEST_CC_MPI} -DHAVE_CONFIG_H -I.      -g -O2 -MT ninja_test_pingpong.o -MD -MP  -lmpi -c -o ninja_test_pingpong.o ninja_test_pingpong.c
${TEST_CC_MPI}   -g -O2   -o ninja_test_pingpong ninja_test_pingpong.o ninja_test_util.o  -lmpi
