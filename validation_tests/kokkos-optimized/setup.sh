#!/bin/bash
. ../../setup.sh
#spack load libpciaccess
#spack load libiconv
#spack load numactl
#spack load hwloc
spackLoadUniqueNoR tau@develop
kokkosHash=spackLoadUnique kokkos+openmp
export KOKKOS=`spack location -i ${kokkosHash}`
