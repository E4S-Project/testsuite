#!/bin/bash

# Function to print script usage
print_usage() {
	echo "Usage: $0 [options] <json_file> [key (setup, clean, compile, run{default})] [value (pass{default}, fail, missing)]"
  echo "Options:"
  echo "  -l  Print output on a single line"
  echo "  -c  Print output in csv format (ignore filters)" 
  echo "  -h  Print this help message"
}

# Function to parse JSON and extract the test values
parse_json() {
  local json_file="$1"
  local print_on_single_line="$2"
  local key="$3"
  local value="$4"

  # Read the JSON file
  local json_data=$(cat "$json_file")

  # Check if -l option is passed
  local output_separator=$'\n'
  if [[ "$print_on_single_line" == "-l" ]]; then
    output_separator=" "
  fi

  # Extract relevant entries and filter them
  local filtered_entries=$(echo "$json_data" | jq -c --arg key "$key" --arg value "$value" '.[] | select(.test_stages[$key] == $value and .test != null)')

  # Loop through each filtered entry and print the directory path
  while IFS= read -r entry; do
    local test_value=$(echo "$entry" | jq -r '.test')
    local directory_path=$(basename "$test_value")
    echo -n "$directory_path$output_separator"
  done <<< "$filtered_entries"

  echo # Print a new line after the output
}

to_csv(){
local json_file="$1"
#echo $json_file
jq -r 'map(select(.test != null)) | .[] | [.test, (.test_stages | to_entries[-1].value), if (.test_stages | to_entries[-1].value) == "fail" then (.test_stages | to_entries[-1].key) else "" end] | @csv' $json_file | tr -d '"' | tr -d '/'

#csv=`jq -r 'map(select(.test != null)) | .[] | [.test, (.test_stages | to_entries[-1].value), if (.test_stages | to_entries[-1].value) == "fail" then (.test_stages | to_entries[-1].key) else "" end] | @csv' $json_file`
#echo $csv
}


# Check if -h option is passed
if [[ "$1" == "-h" || "$1" == "--help" ]]; then
  print_usage
  exit 0
fi

# Check if a JSON file argument is provided
if [[ -z "$1" ]]; then
  print_usage
  exit 1
fi

# Check if -l option is passed
if [[ "$1" == "-c" ]]; then
  if [[ -n "$2" ]]; then
    to_csv "$2"
  else
    print_usage
    exit 1
  fi
  exit 0
fi
if [[ "$1" == "-l" ]]; then
  if [[ -n "$2" ]]; then
    parse_json "$2" "-l" "${3:-run}" "${4:-pass}"
  else
    print_usage
    exit 1
  fi
else
  if [[ -n "$1" ]]; then
    parse_json "$1" "" "${2:-run}" "${3:-pass}"
  else
    print_usage
    exit 1
  fi
fi
