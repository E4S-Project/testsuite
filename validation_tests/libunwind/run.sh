#!/bin/bash
. ./setup.sh
export LD_LIBRARY_PATH=$LIBUNWIND_ROOT/lib:$LD_LIBRARY_PATH
./backtrace
 
