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

./job2job-client

