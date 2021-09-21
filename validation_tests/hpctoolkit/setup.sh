#!/bin/bash
. ../../setup.sh

SPACK_ROOT=`spack location -r`
. ${SPACK_ROOT}/share/spack/setup-env.sh

spack install hpctoolkit@develop
spack load hpctoolkit@develop

