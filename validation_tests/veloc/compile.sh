#!/bin/bash

. ./setup.sh
mpicc -o heatdis_mem heatdis_mem.c -lveloc-client -lm
