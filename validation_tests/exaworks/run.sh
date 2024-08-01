#!/bin/bash
. ./setup.sh
set -e
#set -x

spack find -dvl /$EXAWORKS_HASH
