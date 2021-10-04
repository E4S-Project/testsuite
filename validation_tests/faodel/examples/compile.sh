#!/bin/bash 

# TODO: kelpie  lunasa  nnti  old  opbox  README.md  services  whookie

headdir=$(pwd)

for DIR in common/bootstrap common/data_types common/info_interface common/logging_interface common/singleton  ; do
    cd $DIR
    make
    cd $headdir
done
