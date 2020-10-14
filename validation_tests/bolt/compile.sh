#!/bin/bash
. ./setup.sh
eval $TEST_CC sample_nested.c -lomp
