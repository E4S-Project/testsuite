#!/bin/bash
. ../../setup.sh
THISDIR=`basename "$PWD"`
spackLoadUnique $THISDIR
if [ -n "$SINGULARITY_CONTAINER" ] || [ -n "$APPTAINER_CONTAINER" ] || [ -d "/.singularity.d" ]; then
    echo "[Info] Singularity detected: Restricting Mercury to loopback interface."
    export FI_TCP_IFACE=lo
    export HG_UTIL_USE_HUGE_PAGE=0
fi
