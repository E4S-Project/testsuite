export TAU_OPTIONS="-optVerbose -optRevert -optShared"
export TEST_CC_MPI=mpicc
export TEST_CXX_MPI=mpicxx
export TEST_FTN_MPI=mpif90
export TEST_CC=gcc
export CC=gcc
export TEST_CXX=g++
export CXX=g++
export TEST_FTN=gfortran
export FTN=gfortran
export FC=gfortran
export TEST_RUN_CMD=mpirun
export TEST_RUN_PROCFLAG="-np"
export TEST_RUN_PROCARG="8"
export TEST_RUN="$TEST_RUN_CMD $TEST_RUN_PROCFLAG $TEST_RUN_PROCARG"
#export TEST_ROCM_ARCH="amdgpu_target=gfx908"
#export TEST_CUDA_ARCH="cuda_arch=80"
#export TESTSUITE_VARIANT="%gcc"
