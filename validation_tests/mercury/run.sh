#!/bin/bash
. ./setup.sh
set -x

# Start the server in the background and redirect its output to a file
stdbuf -o0 ./build/src/hg_example_rpc_server > server_output.txt &
server_pid=$!
sleep 2 

# Extract the server address from the server's output
server_address=$(grep -o '".*"' server_output.txt | tr -d '"')

echo $server_address
# Start the client in the background with the server address as an argument
./build/src/hg_example_rpc_client "$server_address" 
outid=$?
kill -6 $server_pid
exit $outid
