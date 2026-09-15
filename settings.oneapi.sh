export TAU_OPTIONS="-optVerbose -optRevert -optShared"
export TEST_CC_MPI=mpiicx
export TEST_CXX_MPI=mpiicpx
export TEST_FTN_MPI=mpiifx
export TEST_CC=icx
export TEST_CXX=icpx
export TEST_FTN=ifx
export CC=icx
export CXX=icpx
export I_MPI_CC=icx
export I_MPI_CXX=icpx
export I_MPI_F77=ifx
export I_MPI_F90=ifx
export TEST_RUN_CMD=mpirun
export TEST_RUN_PROCFLAG="-np"
export TEST_RUN_PROCARG="4"
export TEST_RUN="$TEST_RUN_CMD $TEST_RUN_PROCFLAG $TEST_RUN_PROCARG"


# Limit threaded applications
export OMP_NUM_THREADS=4
                                                                                                                                     export OPENBLAS_NUM_THREADS=2                                                                                                        export MKL_NUM_THREADS=2
export GOTO_NUM_THREADS=2                                                                                                            export VECLIB_MAXIMUM_THREADS=2
export BLIS_NUM_THREADS=2
export TBB_NUM_THREADS=2
export VTK_SMP_MAX_THREADS=2
