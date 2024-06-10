#!/bin/bash
. ./setup.sh
set -x
set -e
cp -r $VARIORUM_ROOT/examples/using-with-make/cpp/* .
make  
