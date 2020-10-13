. ../../setup.sh

kokkosHash=$(spackLoadUniqueNoR kokkos+openmp)
kokkoskernelsHash=$(spackLoadUniqueNoR kokkos-kernels+openmp)
export KOKKOS=`spack location -i ${kokkosHash}`
export KOKKOSKERNELS=`spack location -i ${kokkoskernelsHash}`

