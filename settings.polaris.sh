export TAU_OPTIONS="-optVerbose -optRevert -optShared"
export TEST_CC=/opt/cray/pe/gcc/11.2.0/bin/gcc #cc
export TEST_CXX=/opt/cray/pe/gcc/11.2.0/bin/g++ #CC
export TEST_FTN=/opt/cray/pe/gcc/11.2.0/bin/gfortran #ftn
export TEST_CC_MPI=/opt/cray/pe/mpich/8.1.25/ofi/gnu/9.1/bin/mpicc #cc
export TEST_CXX_MPI=/opt/cray/pe/mpich/8.1.25/ofi/gnu/9.1/bin/mpicxx #CC
export TEST_FTN_MPI=/opt/cray/pe/mpich/8.1.25/ofi/gnu/9.1/bin/mpif90 #ftn
export TEST_RUN_CMD=aprun
export TEST_RUN_PROCFLAG="-n"
export TEST_RUN_PROCARG="8"
export TEST_RUN="$TEST_RUN_CMD $TEST_RUN_PROCFLAG $TEST_RUN_PROCARG"
#Some environments may require even sequential executables to be launched with a command ((e.g. srun for slurm)
#export TEST_RUN_SEQ="$TEST_RUN_CMD"
#export TEST_BLAS_LINK="-L/opt/cray/pe/libsci/22.11.1.2/GNU/9.1/x86_64/lib/ -lsci_gnu"
#export TEST_CUDA_LINK="-L/global/cfs/cdirs/m3896/shared/ParaTools/E4S/23.02/PrgEnv-gnu/spack/opt/spack/linux-sles15-zen3/gcc-11.2.0/cuda-11.8.0-y7swebl4ppluqezejtcxndznp422c4kk/lib64 -lcudart -lcublas -lcusolver"
#export MPICH_GPU_SUPPORT_ENABLED=0
export TESTSUITE_VARIANT="%gcc"
