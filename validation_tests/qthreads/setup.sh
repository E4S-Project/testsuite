#!/bin/bash
. ../../setup.sh
#spack load libiconv
#spack load libpciaccess
#spack load numactl
#spack load hwloc
spackLoadUnique qthreads
#ONEQTHREADS=`spackLoadUnique qthreads`
#export QTHREADS_ROOT=`spack location --install-dir $ONEQTHREADS`
#echo $QTHREADS_ROOT
