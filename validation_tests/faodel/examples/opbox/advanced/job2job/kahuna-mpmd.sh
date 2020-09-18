#!/bin/bash

# process args
TRANSPORT=$1        ; shift
SERVER_NP=$1        ; shift
CLIENT_NP=$1        ; shift
SERVER_NPERNODE=$1  ; shift
CLIENT_NPERNODE=$1  ; shift
SERVER_HOSTS=$1     ; shift
CLIENT_HOSTS=$1     ; shift

function usage
{
  echo "$0 <mpi|ibverbs> <num_servers> <num_clients> [servers_per_node] [clients_per_node] [server_hosts_to_run_on] [client_hosts_to_run_on]"
  exit
}

if [[ "$TRANSPORT" = "" || ( "$TRANSPORT" != "mpi" && "$TRANSPORT" != "ibverbs" ) ]] ; then
  usage
fi
if [ "$NP" = "" ] ; then
  usage
fi

export CONFIG=kahuna.${TRANSPORT}.conf

SERVER_NP_ARGS="-np $SERVER_NP"

CLIENT_NP_ARGS="-np $CLIENT_NP"

if [ "$SERVER_NPERNODE" != "" ] ; then
  SERVER_NPERNODE_ARGS="-npernode $SERVER_NPERNODE"
fi

if [ "$CLIENT_NPERNODE" != "" ] ; then
  CLIENT_NPERNODE_ARGS="-npernode $CLIENT_NPERNODE"
fi

if [ "$SERVER_HOSTS" != "" ] ; then
  SERVER_HOSTS_ARGS="-H $SERVER_HOSTS"
fi

if [ "$CLIENT_HOSTS" != "" ] ; then
  CLIENT_HOSTS_ARGS="-H $CLIENT_HOSTS"
fi

# clean up any old runs
rm server_nodeids

mpirun ${SERVER_HOSTS_ARGS} ${SERVER_NPERNODE_ARGS} ${SERVER_NP_ARGS} ./job2job-server : ${CLIENT_HOSTS_ARGS} ${CLIENT_NPERNODE_ARGS} ${CLIENT_NP_ARGS} ./job2job-client

