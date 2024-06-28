#!/bin/bash
. ./setup.sh
set -x
set -e
mkdir -p ./unify-share-dir
unifyfs start --cleanup --share-dir=./unify-share-dir
set +e
${TEST_RUN} $UNIFYFS_ROOT/libexec/write-gotcha -f testfile
writeret=$?
${TEST_RUN} $UNIFYFS_ROOT/libexec/read-gotcha -f testfile
readret=$?
unifyfs terminate --share-dir=./unify-share-dir

exit writeret+readret
