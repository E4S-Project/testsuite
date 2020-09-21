#!/bin/bash

headdir=$(pwd)

for DIR in common/bootstrap common/data_types common/info_interface common/logging_interface common/singleton ; do
    cd $DIR
    make clean
    cd $headdir
done
