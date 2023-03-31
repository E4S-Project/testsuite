#!/bin/bash
. ./setup.sh
set -x
cd ./mpi_vecadd
make clean
make
