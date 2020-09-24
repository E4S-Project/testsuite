#!/bin/bash

. ./setup.sh
mpirun -np 2 ./heatdis_mem 256 test.cfg
