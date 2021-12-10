#!/bin/bash 
#. ../../setup.sh
. ./setup.sh
set -e
set -x
#if command -v CC &> /dev/null
#then
#	export CXX=CC
#fi
#echo "CXX is ${CXX}"
make  
