#!/bin/bash
. ./setup.sh
set -e
set -x
ncks ./madis-maritime.nc -o test.nc
