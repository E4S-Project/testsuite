#!/bin/bash
. ./setup.sh
set -x
ls -lh --time-style=full-iso /tmp/dir > tmp1.txt
set -e
find_package/build/cpp_example
set +e
ls -lh --time-style=full-iso /tmp/dir > tmp2.txt

diff tmp1.txt tmp2.txt
if diff -q "tmp1.txt" "tmp2.txt" > /dev/null; then
    echo "Error: No change in file written."
    exit 1
fi
