#!/bin/bash
set -x
set -e

# Issue test commands
flux uptime
flux resource info
flux run --label-io -N2 hostname
flux jobs

# Exit the flux shell
exit


