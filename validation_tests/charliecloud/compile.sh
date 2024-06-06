#!/bin/bash
. ./setup.sh
set -x
set -e

cp -r $CHARLIECLOUD_ROOT/share/doc/charliecloud/examples/hello/ .
#cd hello ch-image build .
#ch-convert hello /var/tmp/hello.sqfs
#ch-run /var/tmp/hello.sqfs -- echo "I’m in a container"
