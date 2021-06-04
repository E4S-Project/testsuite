#!/bin/bash

. ./setup.sh

cp -r $GASNET_ROOT/src/tests .
cp Makefile ./tests
cd tests
make

make
