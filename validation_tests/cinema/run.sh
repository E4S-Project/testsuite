#!/bin/bash -e
. ./setup.sh

#
# clone the repository we expect 
#
echo "Cloning repository for $CINEMASCI $CINEMASCI_VERSION ..."
git clone $CINEMASCI_REPOSITORY $CINEMASCI_TEST_DIR
pushd $CINEMASCI_TEST_DIR
git checkout $CINEMASCI_COMMIT
popd

#
# run the unit test
#
echo "Running test ..."
pushd $CINEMASCI_TEST_DIR
python -m unittest testing/test_cdb.py
