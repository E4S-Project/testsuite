#!/bin/bash

. ./setup.sh

cmake -DAscent_ROOT=${ASCENT_ROOT} .
make
