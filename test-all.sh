#!/bin/bash

MIN_PY_VERSION="3.7"

# Prefer 'python3', fall back to 'python'
if command -v python3 &>/dev/null; then
    PYTHON_CMD="python3"
elif command -v python &>/dev/null; then
    PYTHON_CMD="python"
else
    echo "Error: Could not find 'python3' or 'python' in your PATH." >&2
    exit 1
fi

# Get current version in a MAJOR.MINOR format (e.g., "3.10")
CURRENT_PY_VERSION=$($PYTHON_CMD -c 'import sys; print(f"{sys.version_info.major}.{sys.version_info.minor}")')

# If the lowest is not our minimum, it means the current version is too old.
LOWEST_VERSION=$(printf "%s\n%s" "$MIN_PY_VERSION" "$CURRENT_PY_VERSION" | sort -V | head -n1)

if [[ "$LOWEST_VERSION" != "$MIN_PY_VERSION" ]]; then
    echo "Error: Python version $MIN_PY_VERSION or greater is required. Load a different python or try test-all-classic.sh for bash" >&2
    echo "Found $($PYTHON_CMD --version) at $(command -v $PYTHON_CMD)." >&2
    exit 1
fi

settings_file="settings.sh"
for arg in "$@"; do
	if [[ $arg == --settings=* ]]; then
    		settings_file="${arg#--settings=}"
    		if [[ -f "$settings_file" ]]; then
			export TESTSUITE_SETTINGS_FILE=$settings_file
			echo "Sourced settings from $settings_file"
    		else
			echo "Settings file $settings_file does not exist."
      			exit 1
    		fi
  	fi
done
source $settings_file
source setup.sh
$PYTHON_CMD ./test-all.py "$@"
