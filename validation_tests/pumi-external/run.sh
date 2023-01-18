#!/bin/bash 
. ./setup.sh
set -x
uniform $PUMI_ROOT/share/testdata/pipe.dmg $PUMI_ROOT/share/testdata/pipe.smb pipe_unif.smb
${TEST_RUN_CMD} ${TEST_RUN_PROCFLAG} 2 split $PUMI_ROOT/share/testdata/pipe.dmg $PUMI_ROOT/share/testdata/pipe.smb pipe_2_.smb 2
