#!/bin/bash
source /projects/tau/tau.bashrc
export TAU_OMP_SUPPORT_LEVEL=full
export TAU_OMPT_RESOLVE_ADDRESS_EAGERLY=1
export OMP_PROC_BIND=1
export TAU_TRACE=1
export TAU_TRACE_FORMAT=otf2
tau_exec -T ompt,tr6,serial -ompt ./lulesh.host 
