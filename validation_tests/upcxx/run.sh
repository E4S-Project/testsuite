#!/bin/bash
. ./setup.sh
set -e 
set -x
EXAMPLES=(dmap-test 
dmap-async-insert-test
dmap-async-find-test 
dmap-quiescence-test 
dmap-atomics-test 
dmap-conjoined-test
dmap-promises-test 
rb1d 
hello-world 
view-histogram1 
view-histogram2 
view-matrix-tasks 
non-contig-example 
team_simple 
team_advanced 
persona-example-rputs 
persona-example       
serial-custom 
serial-fields 
serial-recursive 
serial-values)

#echo ${EXAMPLES[@]}
for program in ${EXAMPLES[@]}; do
upcxx-run -n 4 ./${program}
#echo ${program}
done




