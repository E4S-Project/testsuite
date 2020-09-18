#!/bin/bash

# TODO: kelpie  lunasa  nnti  old  opbox  README.md  services  whookie

for DIR in common/bootstrap common/data_types common/info_interface common/logging_interface common/singleton dirman ; do
    cd $DIR
    make
    cd ..
done
