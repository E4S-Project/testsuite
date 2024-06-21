#!/bin/bash
. ./setup.sh
set -x
./build/minimal/symb_look
./build/autotee/autotee_test
