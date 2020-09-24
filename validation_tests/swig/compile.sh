#!/bin/bash
. ./setup.sh
swig -tcl example.i
gcc -fPIC -c example.c example_wrap.c -I/usr/include
gcc -shared example.o example_wrap.o -o example.so
