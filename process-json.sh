#!/bin/bash

# Function to print script usage
print_usage() {
  echo "Usage: $0 [options] <json_file> [key (setup, clean, compile, run{default})] [value (pass{default}, fail, missing)]"
  echo "Options:"
  echo "  -s, --stats  Print vital statistics after each test"
  echo "  -l           Print output on a single line"
  echo "  -c           Print output in csv format (ignore filters)" 
  echo "  -h, --help   Print this help message"
}

# Function to parse JSON and extract the test values
parse_json() {
  local json_file="$1"
  local print_on_single_line="$2"
  local key="$3"
  local value="$4"
  local show_stats="$5"

  # Extract relevant entries and filter them
  local filtered_entries=$(jq -c --arg key "$key" --arg value "$value" '
    .[] | select(
      .test != null and
      (.test_stages[$key] == $value or ($value == "fail" and .test_stages[$key] == "timeout"))
    )
  ' "$json_file")

  # Loop through each filtered entry and print the directory path
  while IFS= read -r entry; do
    [[ -z "$entry" ]] && continue
    local test_value=$(echo "$entry" | jq -r '.test')
    local directory_path=$(basename "$test_value")

    if [[ "$show_stats" == "true" ]]; then
      local stats_str=$(echo "$entry" | jq -r --arg key "$key" '
        (.metrics[$key] // .metrics.run // null) as $m |
        if $m != null then
          "time: \($m.elapsed_sec // 0)s | cpu: \($m.cpu_percent // 0)% | user: \($m.user_cpu_sec // 0)s | sys: \($m.sys_cpu_sec // 0)s | maxrss: \(if ($m.max_rss_kb != null and $m.max_rss_kb > 0) then "\((($m.max_rss_kb / 10.24 | round) / 100))MB" else "0MB" end) | ctx: \($m.vol_ctx_switches // 0)v/\($m.invol_ctx_switches // 0)i"
        else
          "(no metrics)"
        end
      ')

      if [[ "$print_on_single_line" == "-l" ]]; then
        echo -n "$directory_path [$stats_str] "
      else
        echo "$directory_path"
        echo "  $stats_str"
      fi
    else
      if [[ "$print_on_single_line" == "-l" ]]; then
        echo -n "$directory_path "
      else
        echo "$directory_path"
      fi
    fi
  done <<< "$filtered_entries"

  if [[ "$print_on_single_line" == "-l" ]]; then
    echo
  fi
}

to_csv() {
  local json_file="$1"
  local show_stats="$2"

  if [[ "$show_stats" == "true" ]]; then
    jq -r 'map(select(.test != null)) | .[] | [
      .test,
      (.test_stages | to_entries[-1].value),
      (if (.test_stages | to_entries[-1].value) == "fail" then (.test_stages | to_entries[-1].key) else "" end),
      (.metrics.run.elapsed_sec // ""),
      (.metrics.run.cpu_percent // ""),
      (.metrics.run.max_rss_kb // "")
    ] | @csv' "$json_file" | tr -d '"' | tr -d '/'
  else
    jq -r 'map(select(.test != null)) | .[] | [.test, (.test_stages | to_entries[-1].value), if (.test_stages | to_entries[-1].value) == "fail" then (.test_stages | to_entries[-1].key) else "" end] | @csv' "$json_file" | tr -d '"' | tr -d '/'
  fi
}

show_stats=false
print_single_line=""
csv_mode=false
positional_args=()

while [[ $# -gt 0 ]]; do
  case "$1" in
    -h|--help)
      print_usage
      exit 0
      ;;
    -l)
      print_single_line="-l"
      shift
      ;;
    -s|--stats)
      show_stats=true
      shift
      ;;
    -c)
      csv_mode=true
      shift
      ;;
    -*)
      echo "Unknown option: $1" >&2
      print_usage
      exit 1
      ;;
    *)
      positional_args+=("$1")
      shift
      ;;
  esac
done

if [[ ${#positional_args[@]} -eq 0 ]]; then
  print_usage
  exit 1
fi

json_file="${positional_args[0]}"
key="${positional_args[1]:-run}"
value="${positional_args[2]:-pass}"

if [[ ! -f "$json_file" ]]; then
  echo "Error: JSON file '$json_file' does not exist." >&2
  exit 1
fi

if [[ "$csv_mode" == true ]]; then
  to_csv "$json_file" "$show_stats"
  exit 0
fi

parse_json "$json_file" "$print_single_line" "$key" "$value" "$show_stats"
