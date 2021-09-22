#!/bin/bash

. ./setup.sh

cmake \
	-DCMAKE_C_COMPILER=$TEST_CC \
	-DCMAKE_CXX_COMPILER=$TEST_CXX \
	-DCabana_ROOT=Cabana_ROOT .

make
