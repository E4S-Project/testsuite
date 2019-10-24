#!/bin/bash
spack load libpciaccess
spack load libiconv
spack load numactl
spack load hwloc
spack load tau@develop
export KOKKOS=`spack location -i kokkos+openmp`
