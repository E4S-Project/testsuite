#!/bin/bash
. ./setup.sh

#
# clean up, if there is anything that might interfere
#
if [ -d $CINEMASCI_TEST_DIR ]; then
    echo "removing existing test directory: $CINEMASCI_TEST_DIR"
    rm -rf $CINEMASCI_TEST_DIR
fi
