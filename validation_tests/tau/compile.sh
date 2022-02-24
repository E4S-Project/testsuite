#!/bin/bash
. ./setup.sh
#retVal=$?
#if [ $retVal -ne 0 ] ; then
#  exit $retVal
#fi
set -x
set -e
mkdir -p bin
#Fixes possible pdt parser error.
export LC_ALL=C
make # LU 
#retVal=$?
#if [ $retVal -ne 0 ] ; then
#  exit $retVal
#fi
