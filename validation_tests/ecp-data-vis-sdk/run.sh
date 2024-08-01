#!/bin/bash
. ./setup.sh
set -e
#set -x

spack find -dvl /$ECP_DATA_VIS_SDK_HASH
