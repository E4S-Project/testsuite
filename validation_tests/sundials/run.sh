#!/bin/bash -e
. ./setup.sh

if [ "$USECUDA" = "1" ]; then
    EXAMPLE_DIR="cuda"
elif [ "$USEROCM" = "1" ]; then
    EXAMPLE_DIR="hip"
else
    EXAMPLE_DIR="serial"
fi

cd "examples/${EXAMPLE_DIR}"
ctest --output-on-failure

