#!/bin/bash

# process args
TRANSPORT=$1 ; shift
NP=$1        ; shift
NPERNODE=$1  ; shift
HOSTS=$1     ; shift

function usage
{
  echo "$0 <mpi|ibverbs> <num_servers> [servers_per_node] [hosts_to_run_on]"
  exit
}

if [[ "$TRANSPORT" = "" || ( "$TRANSPORT" != "mpi" && "$TRANSPORT" != "ibverbs" ) ]] ; then
  usage
fi
if [ "$NP" = "" ] ; then
  usage
fi

export CONFIG=kahuna.${TRANSPORT}.conf

NP_ARGS="-np $NP"

if [ "$NPERNODE" != "" ] ; then
  NPERNODE_ARGS="-npernode $NPERNODE"
fi

if [ "$HOSTS" != "" ] ; then
  HOSTS_ARGS="-H $HOSTS"
fi

# clean up any old runs
rm server_nodeids

mpirun ${HOSTS_ARGS} ${NPERNODE_ARGS} ${NP_ARGS} ./job2job-server

