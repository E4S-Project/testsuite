#!/bin/bash
. ./setup.sh
set -x
set -e
timeout 3m wannier90.x copper 
cat *.wout
