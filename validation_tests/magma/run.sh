#!/bin/bash -e
. ./setup.sh
set -x
timeout 2m ./example_sparse    
timeout 2m ./example_sparse_operator    
timeout 2m ./example_v1    
timeout 2m ./example_v2


