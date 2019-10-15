#!/bin/bash
. ../../setup.sh
spack load mpich
spack load tau@develop
mkdir -p bin
make LU 
retVal=$?
if [ $retVal -ne 0 ] ; then
  exit $retVal
fi
