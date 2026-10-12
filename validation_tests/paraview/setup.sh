#!/bin/bash
. ../../setup.sh

# Actually testing `paraview`
spackLoadUnique paraview@5.11.0-RC3:

# Also need `cmake`, and `ninja` to build test tests.
spackLoadUnique cmake
spackLoadUnique ninja
