#!/bin/bash
. ../../setup.sh

# Actually testing `libcatalyst`
spackLoadUnique libcatalyst@2.0.0-rc3:

# Also need `cmake`, and `ninja` to build tests.
spackLoadUniqueNoX cmake
spackLoadUnique ninja
