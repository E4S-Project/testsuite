#!/bin/bash

for DIR in common/bootstrap dirman ; do
    cd $DIR
    make clean
    cd ..
done
