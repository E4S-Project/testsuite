#!/bin/bash
. ./setup.sh
set -e
set -x

EXAGO_DIR=$EXAGO_ROOT

 netfile=$EXAGO_DIR/share/exago/datafiles/case9/case9mod.m
opflow -print_output -netfile $netfile

#netfile=$EXAGO_DIR/share/exago/datafiles/case9/case9mod.m
#ctgcfile=$EXAGO_DIR/share/exago/datafiles/case9/case9.cont

#cat $ctgcfile


#mpiexec -n 4 $EXAGO_DIR/bin/opflow -netfile $netfile -ctgcfile $ctgcfile -scopflow_Nc -1 -scopflow_solver EMPAR -print_output -save_output -scopflow_output_directory case9scopf -opflow_initialization ACPF -opflow_output_format MATPOWER
