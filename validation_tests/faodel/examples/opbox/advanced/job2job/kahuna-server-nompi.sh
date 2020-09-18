#!/bin/bash

# process args
TRANSPORT=$1 ; shift

function usage
{
  echo "$0 <mpi|ibverbs>
  exit
}

if [[ "$TRANSPORT" = "" || ( "$TRANSPORT" != "mpi" && "$TRANSPORT" != "ibverbs" ) ]] ; then
  usage
fi

export CONFIG=kahuna.${TRANSPORT}.conf

# clean up any old runs
rm server_nodeids

./job2job-server

