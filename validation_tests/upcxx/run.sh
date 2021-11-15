#!/bin/bash
. ./setup.sh
set -e 
set -x

cd example/prog-guide

make run PROCS=4


