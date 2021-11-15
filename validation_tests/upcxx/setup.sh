#!/bin/bash
. ../../setup.sh
spackLoadUnique upcxx #+internal-superlu
#spackLoadUnique openblas threads=openmp

# Default to UPC++'s portable smp-conduit backend, unless one was specified.
# This might differ from the default conduit for the UPC++ install,
# which might be a distributed conduit with special spawning requirements.
# The goal is to test multi-process without spawning problems.
export UPCXX_NETWORK=${UPCXX_NETWORK:-smp}

# This directs udp-conduit (if in use) to default to spawn all processes locally:
export GASNET_SPAWNFN=${GASNET_SPAWNFN:-L}
