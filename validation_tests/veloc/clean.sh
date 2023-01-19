#!/bin/bash

rm -rf heatdis_fault #/tmp/scratch /tmp/persistent
rm -rf  /dev/shm/scratch/heatdis*
rm -rf /dev/shm/persistent/heatdis*
echo "You may need to alter clean.sh if veloc does not use /dev/shm on your system"

