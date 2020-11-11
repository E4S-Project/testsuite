#!/bin/bash

. ./setup.sh

swig -python example.i

$TEST_CC -fpic -c example_wrap.c example.c
$TEST_CC -shared -o _example.so example_wrap.o example.o
