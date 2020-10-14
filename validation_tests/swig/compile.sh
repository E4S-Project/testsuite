#!/bin/bash
. ./setup.sh
swig -tcl example.i
#gcc 
${TEST_CC} -fPIC -c example.c example_wrap.c -I/usr/include
#gcc
${TEST_CC} -shared example.o example_wrap.o -o example.so
