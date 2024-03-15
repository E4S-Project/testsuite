#!/bin/bash
. ../../setup.sh
spackLoadUniqueNoX cmake@3.17:

spackLoadUnique hpx networking=mpi
