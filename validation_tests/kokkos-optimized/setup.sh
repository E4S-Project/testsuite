#!/bin/bash
. ../../setup.sh
#spack load libpciaccess
#spack load libiconv
#spack load numactl
#spack load hwloc
spackLoadUniqueNoR tau #@develop
#spackLoadUnique kokkos
kokkosHash=$(spackLoadUnique kokkos) #+openmp
spackLoadUnique kokkos
#spack load -r kokkos
#module list

export KOKKOS=`spack location -i ${kokkosHash}`
