#!/bin/bash
. ../../setup.sh
spackLoadUnique hypre+internal-superlu
spackLoadUnique openblas threads=openmp
