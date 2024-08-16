#!/bin/bash
. ./setup.sh
#set -e
set -x

# Start the flux daemon in a new shell
flux start --test-size=2 ./fluxrun.sh 

# Check the exit status of the flux commands
if [ $? -eq 0 ]; then
    echo "Flux test passed"
else
    echo "Flux test failed"
fi
