#!/bin/bash
source /projects/tau/tau.bashrc
export TAU_OMP_SUPPORT_LEVEL=full
export TAU_OMPT_RESOLVE_ADDRESS_EAGERLY=1
export TAU_CALLPATH=1
export TAU_CALLPATH_DEPTH=100
export OMP_PROC_BIND=1
tau_exec -T ompt,tr6,serial -ompt -ebs  ./lulesh.host 


