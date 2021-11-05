#!/bin/bash -e
. ./setup.sh
set -x
$TEST_RUN_CMD $TEST_RUN_PROCFLAG 1 ./build/test_future --hpx:use-process-mask
$TEST_RUN_CMD $TEST_RUN_PROCFLAG 1 ./build/test_algorithm --hpx:use-process-mask
$TEST_RUN_CMD $TEST_RUN_PROCFLAG 1 ./build/test_gpu --hpx:use-process-mask

$TEST_RUN ./build/test_distributed --hpx:use-process-mask > test_distributed.log
sort test_distributed.log > test_distributed_sorted.log
rm -f test_distributed_reference.log
for i in $(seq 0 $((TEST_RUN_PROCARG - 1))); do
    echo "Hello world from locality $i" >> test_distributed_reference.log
done
diff test_distributed_sorted.log test_distributed_reference.log
