#!/bin/bash 
set -e
. ./setup.sh
set -x
./a.out || exit 1
