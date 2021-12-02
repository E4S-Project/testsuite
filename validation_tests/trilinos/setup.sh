#!/bin/bash
. ../../setup.sh
spackLoadUnique trilinos+cuda cuda_arch=80
spackLoadUnique cmake
