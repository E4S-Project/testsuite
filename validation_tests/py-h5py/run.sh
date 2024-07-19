#!/bin/bash
. ./setup.sh
set -e
set -x
python ./write-read.py
ls -lh ./data.h5
 
