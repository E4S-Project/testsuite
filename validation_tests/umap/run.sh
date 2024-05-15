#!/bin/bash 
. ./setup.sh
set -x

# This must be equal to one for umap to run
umap_output=$(cat /proc/sys/vm/unprivileged_userfaultfd 2>/dev/null)

# Check if the command was successful
if [ $? -ne 0 ]; then
    echo "Test Halted: Couldn't check unprivileged_userfaultfd"
    exit 1
fi

# Check if the output is equal to one
if [ "$umap_output" -eq 1 ]; then
    echo "unprivileged_userfaultfd ok"
else
    echo "Test Halted: unprivileged_userfaultfd is not equal to one"
    exit 1
fi

./umaptest
