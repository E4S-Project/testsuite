#!/bin/bash
. ../../setup.sh
THISDIR=pumi #`basename "$PWD"`
spackLoadUnique $THISDIR

E4S_SPACK_TEST_HASH_NAME=${THISDIR^^}_HASH
export E4S_SPACK_TEST_HASH=${!E4S_SPACK_TEST_HASH_NAME}
