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
  echo "$0 <mpi|ugni> <num_servers> <num_clients> [servers_per_node] [clients_per_node] [server_hosts_to_run_on] [client_hosts_to_run_on]"
  exit
}

if [[ "$TRANSPORT" = "" || ( "$TRANSPORT" != "mpi" && "$TRANSPORT" != "ugni" ) ]] ; then
  usage
fi
if [ "$SERVER_NP" = "" ] ; then
  usage
fi
if [ "$CLIENT_NP" = "" ] ; then
  usage
fi

export CONFIG=mutrino.${TRANSPORT}.conf

SERVER_NP_ARGS="-n $SERVER_NP"

CLIENT_NP_ARGS="-n $CLIENT_NP"

if [ "$SERVER_NPERNODE" != "" ] ; then
  SERVER_NPERNODE_ARGS="-N $SERVER_NPERNODE"
fi

if [ "$CLIENT_NPERNODE" != "" ] ; then
  CLIENT_NPERNODE_ARGS="-N $CLIENT_NPERNODE"
fi

if [ "$SERVER_HOSTS" != "" ] ; then
  SERVER_HOSTS_ARGS="-L $SERVER_HOSTS"
fi

if [ "$CLIENT_HOSTS" != "" ] ; then
  CLIENT_HOSTS_ARGS="-L $CLIENT_HOSTS"
fi

# clean up any old runs
rm server_nodeids

aprun ${SERVER_HOSTS_ARGS} ${SERVER_NPERNODE_ARGS} ${SERVER_NP_ARGS} ./job2job-server : ${CLIENT_HOSTS_ARGS} ${CLIENT_NPERNODE_ARGS} ${CLIENT_NP_ARGS} ./job2job-client

