. ../../setup.sh

spackLoadUniqueNoR kokkos+openmp
spackLoadUniqueNoR kokkos-kernels+openmp
#kokkosHash=$(spackLoadUniqueNoR kokkos+openmp)
#kokkoskernelsHash=$(spackLoadUniqueNoR kokkos-kernels+openmp)
export KOKKOS=KOKKOS_ROOT #`spack location -i ${kokkosHash}`
export KOKKOSKERNELS=KOKKOS_KERNELS_ROOT #`spack location -i ${kokkoskernelsHash}`

