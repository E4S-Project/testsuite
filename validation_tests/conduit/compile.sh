#!/bin/bash

. ./setup.sh

cmake -DConduit_ROOT=${CONDUIT_ROOT} .
make
