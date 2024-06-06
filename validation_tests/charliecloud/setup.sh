#!/bin/bash
. ../../setup.sh
spackLoadUnique charliecloud 
if [ -z ${USER+x} ]; then export USER=`whoami`;fi
