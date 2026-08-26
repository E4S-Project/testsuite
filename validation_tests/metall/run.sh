#!/bin/bash
. ./setup.sh
set -x

export METALL_TEST_DIR=$(mktemp -d -p /tmp metall_XXXXXX)

ls -lh --time-style=full-iso "$METALL_TEST_DIR" > tmp1.txt

set -e
find_package/build/cpp_example
set +e

ls -lh --time-style=full-iso "$METALL_TEST_DIR" > tmp2.txt

diff tmp1.txt tmp2.txt
if diff -q "tmp1.txt" "tmp2.txt" > /dev/null; then
    echo "Error: No change in file written."
    rm -rf "$METALL_TEST_DIR"
    exit 1
fi

# Clean up
rm -rf "$METALL_TEST_DIR"
