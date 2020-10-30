#!/bin/bash -e 
. ./setup.sh
set -x
echo "KOKKOS=$KOKKOS"
make
