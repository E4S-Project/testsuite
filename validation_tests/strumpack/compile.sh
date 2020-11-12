#!/bin/bash -e

. ./setup.sh

cmake .
make -j2
