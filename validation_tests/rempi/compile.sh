#!/bin/bash
. ./setup.sh
set -x
set -e

${TEST_CC_MPI} ./example1.c -o example1  
