#!/bin/bash
. ./setup.sh
#retVal=$?
#if [ $retVal -ne 0 ] ; then
#  exit $retVal
#fi
set -x
set -e
mkdir -p bin
make # LU 
#retVal=$?
#if [ $retVal -ne 0 ] ; then
#  exit $retVal
#fi
