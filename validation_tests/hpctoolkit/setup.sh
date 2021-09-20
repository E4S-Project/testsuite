#!/bin/bash
. ../../setup.sh

SPACK_ROOT=`spack location -r`
. ${SPACK_ROOT}/share/spack/setup-env.sh

spack install cuda
spack load cuda /3zjz5ms

spack install hpctoolkit@develop+cuda
spack load hpctoolkit@develop+cuda


