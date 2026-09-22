#!/bin/bash
. ../../setup.sh

THISDIR=$(basename "$PWD")
USECUDA=0
USEROCM=0

if [ "$THISDIR" = "amrex-cuda" ]; then
    spackLoadUnique amrex+cuda
    USECUDA=1
elif [ "$THISDIR" = "amrex-rocm" ]; then
    spackLoadUnique amrex+rocm
    USEROCM=1
else
    spackLoadUnique amrex~rocm~cuda
fi

if ! command -v cmake >/dev/null 2>&1 ; then
    spackLoadUniqueNoX cmake@3.22.2:
fi

# Cap MPI ranks on GPU runs to avoid heavy oversubscription if only 1 GPU is present
if [ "$USECUDA" = "1" ] || [ "$USEROCM" = "1" ]; then
    # Detect GPU count (default to 1 if detection tools aren't present)
    NGPUS=1
    if command -v nvidia-smi >/dev/null 2>&1; then
        NGPUS=$(nvidia-smi -L 2>/dev/null | wc -l)
    elif command -v rocm-smi >/dev/null 2>&1; then
        NGPUS=$(rocm-smi --showid 2>/dev/null | grep -c "GPU")
    fi
    [ "$NGPUS" -lt 1 ] && NGPUS=1

    # If requested ranks > available GPUs, cap ranks to NGPUS (or max 2)
    REQ_RANKS="${TEST_RUN_PROCARG:-8}"
    if [ "$REQ_RANKS" -gt "$NGPUS" ]; then
        # Limit to available GPUs or max 2 ranks for single-GPU validation
        SAFE_RANKS=$(( NGPUS > 2 ? NGPUS : 2 ))
        export TEST_RUN="${TEST_RUN_CMD:-mpirun} ${TEST_RUN_PROCFLAG:--np} ${SAFE_RANKS}"
    fi
fi
