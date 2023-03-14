#!/bin/bash

. ./setup.sh
set -x
swig -python example.i

$TEST_CC -fpic -c example_wrap.c example.c `python-config --includes` 
$TEST_CC -shared -o _example.so example_wrap.o example.o
