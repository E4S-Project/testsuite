#!/bin/bash
. ./setup.sh
set -x
set -e
cd hello 
ch-image build .
ch-convert hello /var/tmp/hello #.sqfs
ch-run /var/tmp/hello -- echo "I’m in a container"
 
