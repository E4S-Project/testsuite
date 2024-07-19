#!/bin/bash
. ./setup.sh
set -e
set -x
#${TEST_RUN} ./lu.W.4
cp -r $OPENFOAM_ROOT/tutorials/incompressible/icoFoam/cavity/cavity .
cd cavity
blockMesh
icoFoam
 
