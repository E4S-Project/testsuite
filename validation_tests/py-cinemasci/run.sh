#!/bin/bash
. ./setup.sh
set -e
set -x
python ./cdbtest.py
cat my_database.cdb/data.csv
