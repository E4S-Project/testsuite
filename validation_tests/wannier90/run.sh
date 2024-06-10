#!/bin/bash
. ./setup.sh
set -x
set -e
wannier90.x copper 
cat *.wout
