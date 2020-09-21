#!/bin/bash

# process args
TRANSPORT=$1 ; shift
PDOMAIN=$1   ; shift
NP=$1        ; shift
NPERNODE=$1  ; shift
HOSTS=$1     ; shift

function usage
{
  echo "$0 <mpi|ugni> <pdomain_name> <num_clients> [clients_per_node] [hosts_to_run_on]"
  exit
}

if [[ "$TRANSPORT" = "" || ( "$TRANSPORT" != "mpi" && "$TRANSPORT" != "ugni" ) ]] ; then
  usage
fi
if [ "$PDOMAIN" = "" ] ; then
  usage
fi
if [ "$NP" = "" ] ; then
  usage
fi

export CONFIG=mutrino.${TRANSPORT}.conf

PDOMAIN_ARGS="-p $PDOMAIN"

NP_ARGS="-n $NP"

if [ "$NPERNODE" != "" ] ; then
  NPERNODE_ARGS="-N $NPERNODE"
fi

if [ "$HOSTS" != "" ] ; then
  HOSTS_ARGS="-L $HOSTS"
fi

aprun ${PDOMAIN_ARGS} ${HOSTS_ARGS} ${NPERNODE_ARGS} ${NP_ARGS} ./job2job-client

