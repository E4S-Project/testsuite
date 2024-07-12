#!/bin/bash
# Echoes the command line to invoke nccmp.
# Has option to run memory checker.

if test "$MEMCHECK" = "1"; then 
    echo valgrind --tool=memcheck --leak-check=full --show-reachable=yes --log-file=tmp.valgrind --num-callers=20 --track-origins=yes --freelist-vol=100000000 --freelist-big-blocks=10000000 --malloc-fill=A --free-fill=F nccmp #../src/nccmp
else 
    echo nccmp #../src/nccmp
fi
