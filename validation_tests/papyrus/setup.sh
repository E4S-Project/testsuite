#!/bin/bash
. ../../setup.sh
spackLoadUnique  papyrus

# Check if running inside Singularity or Apptainer
if [ -n "$SINGULARITY_CONTAINER" ] || [ -n "$APPTAINER_CONTAINER" ] || [ -d "/.singularity.d" ]; then
    echo "[Info] Singularity detected: Restricting Papyrus/MPI to loopback interface."
    export FI_TCP_IFACE=lo
    export UCX_NET_DEVICES=lo
    export OMPI_MCA_btl_tcp_if_include=lo
    export HYDRA_IFACE=lo
fi
