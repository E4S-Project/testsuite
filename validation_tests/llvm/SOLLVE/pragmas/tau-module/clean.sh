#!/bin/bash

for n in "tmp_files" "ytopt.log" "profile.0.0.0"  "profile.0.0.1"  "results.csv"  "results.json" "__pycache__" ; do
    find . -name $n -exec rm -rf {} \;
done
