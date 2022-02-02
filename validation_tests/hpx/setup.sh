#!/bin/bash
. ../../setup.sh
spackLoadUnique cmake@3.17:

spackLoadUnique hpx networking=mpi
