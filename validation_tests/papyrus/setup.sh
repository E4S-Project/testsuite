#!/bin/bash
. ../../setup.sh
spackLoadUnique  papyrus

# Check if running inside Singularity or Apptainer
#if [ -n "$SINGULARITY_CONTAINER" ] || [ -n "$APPTAINER_CONTAINER" ] || [ -d "/.singularity.d" ]; then
#    echo "[Info] Singularity detected: Restricting Papyrus/MPI to loopback interface."
#    export FI_TCP_IFACE=lo
#    export UCX_NET_DEVICES=lo
#    export OMPI_MCA_btl_tcp_if_include=lo
#    export HYDRA_IFACE=lo
#fi
# Check if we are outside a batch scheduler allocation
if [ -z "$SLURM_NODELIST" ] && [ -z "$PBS_NODEFILE" ] && [ -z "$LSB_HOSTS" ]; then
    echo "[Info] No multi-node allocation detected. Restricting Papyrus/MPI to loopback interface."
    export FI_TCP_IFACE=lo
    export UCX_NET_DEVICES=lo
    export OMPI_MCA_btl_tcp_if_include=lo
    export HYDRA_IFACE=lo
fi
