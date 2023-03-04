#!/bin/bash

. ./setup.sh

cmake \
	-DCMAKE_C_COMPILER=$ARBORX_CC \
	-DCMAKE_CXX_COMPILER=$ARBORX_CXX \
	-DArborX_ROOT=ARBORX_ROOT .

make
