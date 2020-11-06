#!/bin/bash

. ./setup.sh

cmake -DArborX_ROOT=ARBORX_ROOT .
make
