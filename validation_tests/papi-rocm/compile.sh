#!/bin/bash
. ./setup.sh
set -e
set -x

make PAPIROOT=$PAPI_ROOT
