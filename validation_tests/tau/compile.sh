#!/bin/bash
. ./setup.sh
#retVal=$?
#if [ $retVal -ne 0 ] ; then
#  exit $retVal
#fi
set -x

touch empty.f90
${TEST_FTN} -c -fallow-argument-mismatch  empty.f90
AAM_test=$?
if [ $AAM_test -eq 0 ];then
	export E4S_AAM="-fallow-argument-mismatch" 
fi

set -e
mkdir -p bin

#Fixes possible pdt parser error.
export LC_ALL=C
make # LU 
ls ./bin/lu.W.4
#retVal=$?
#if [ $retVal -ne 0 ] ; then
#  exit $retVal
#fi
