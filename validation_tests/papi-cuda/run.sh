#!/bin/bash
. ./setup.sh
#set -e
#set -x

./test_cuda/HelloWorld cuda:::dram__bytes_read.sum:device=0 cuda:::sm__cycles_active.sum:device=0 cuda:::smsp__warps_launched.sum:device=0
