#!/bin/bash 
. ./setup.sh

if [ ! -z $CRAY_LD_LIBRARY_PATH ] ; then
	echo "On Cray system. Checking for +mpispawn"
	spack find -v /${E4S_SPACK_TEST_HASH} | grep "+mpispawn"
	cray_mpi_res=$?
	if [ ${cray_mpi_res} -eq 0 ];then
		echo "+mpispawn found. Variant invalid with Cray."
		exit 1
	fi
fi
spackTestRun $E4S_SPACK_TEST_HASH
