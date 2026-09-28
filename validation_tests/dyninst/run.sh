#!/bin/bash -e
. ./setup.sh

cd xmas_tree/build
./g_xmas_tree

expected=$(./hello_target)

out=$(parseThat --binary-edit=./hello_target.rw -p 2 -i 0 ./hello_target 2>&1)
echo "$out"

echo "$out" | grep -Eq 'Found [1-9][0-9]* function' \
  || { echo "FAIL: no functions parsed"; exit 1; }
echo "$out" | grep -q 'mutator exited normally and returned 0' \
  || { echo "FAIL: mutator did not exit cleanly"; exit 1; }
[ -s hello_target.rw ] \
  || { echo "FAIL: rewritten binary not produced"; exit 1; }


actual=$(./hello_target.rw)
[ "$actual" = "$expected" ] \
  || { echo "FAIL: rewritten binary output '$actual' != '$expected'"; exit 1; }


echo "PASS"
