#!/bin/bash

. ./setup.sh

$TEST_CC -o hello-world hello-world.c -I${AML_ROOT}/include -L${AML_ROOT}/lib  -laml
