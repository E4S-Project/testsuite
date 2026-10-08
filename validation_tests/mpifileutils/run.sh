#!/bin/bash
. ./setup.sh
set -e
#set -x

rm -rf ./test-src ./test
mkdir -p test-src test
echo "Generating test directories/files"
# Build a synthetic tree: 20 dirs x 10 subdirs x 25 files = 5000 files
for d in $(seq 1 20); do
  for s in $(seq 1 10); do
    dir=test-src/d$d/s$s
    mkdir -p "$dir"
    for f in $(seq 1 25); do
      # ~16 KiB of non-trivial data per file
      head -c 16384 /dev/urandom > "$dir/f$f.dat"
    done
  done
done
set -x
# A few symlinks and a bigger file so dcp exercises those paths too
mkdir -p test-src/empty-dir
: > test-src/empty-file
echo hi > "test-src/name with spaces.txt"
chmod 600 test-src/d1/s1/f2.dat
chmod 755 test-src/d1/s1/f3.dat
truncate -s 200M test-src/sparse.bin
ln -s d1/s1/f1.dat test-src/link-to-file
ln -s d1 test-src/link-to-dir
head -c 64M /dev/urandom > test-src/big.bin
echo "Generated $(find test-src -type f | wc -l) files"
${TEST_RUN} dcp ./test-src ./test
${TEST_RUN} dcmp ./test-src ./test/test-src
${TEST_RUN} dwalk -p ./test
${TEST_RUN} drm ./test
${TEST_RUN} drm ./test-src
