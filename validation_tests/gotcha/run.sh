#!/bin/bash
. ./setup.sh
set -x
./build/minimal/symb_look
script -q -e -c "./build/autotee/autotee_test"
