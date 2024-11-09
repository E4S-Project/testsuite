#!/bin/bash

# Check if at least the directory name argument is provided
if [ $# -lt 1 ]; then
  echo "Usage: $0 [log_prefix] <directory_name>"
  exit 1
fi

# Set the log prefix and directory name based on the number of arguments
if [ $# -eq 2 ]; then
  log_prefix=$1-
  dir=$2
else
  log_prefix="run-"
  dir=$1
fi

# Define the log directory path
log_path="./validation_tests/$dir"

# Check if the directory exists
if [ ! -d "$log_path" ]; then
  echo "Directory $log_path does not exist."
  exit 1
fi

# Find the most recent log file with the pattern '<log_prefix><dir>_*.log'
latest_log=$(ls -t "$log_path"/"$log_prefix$dir"_*.log 2>/dev/null | head -n 1)

# Check if any log files were found
if [ -z "$latest_log" ]; then
  echo "No log files matching '$log_prefix$dir_*.log' were found in $log_path."
  exit 1
fi

# Display the contents of the most recent log file
vim "$latest_log"
