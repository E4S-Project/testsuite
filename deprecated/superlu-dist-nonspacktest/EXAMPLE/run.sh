#!/bin/bash

#. ../setup.sh
OMP_NUM_THREADS=4; ${TEST_RUN} ./pddrive -r 2 -c 2 g20.rua
