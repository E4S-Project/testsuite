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

#What the system is using before we set up the test environment
echo "=== Pre-test Compilers ==="
#echo "  PATH: $PATH"
for var in CC CXX FTN FC; do
    val="${!var}"
    if [ -n "$val" ]; then
        printf '  %-4s (pre-set) = %-10s -> %s\n' "$var" "$val" "$(command -v "$val" 2>/dev/null || echo NOT_FOUND)"
    fi
done
echo "  base cc  -> $(command -v cc  2>/dev/null || echo NOT_FOUND)"
echo "  base c++ -> $(command -v c++ 2>/dev/null || echo NOT_FOUND)"
echo "  base gcc -> $(command -v gcc 2>/dev/null || echo NOT_FOUND)"
echo "============================="

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


echo "=== Test Environment Summary ==="

# Where are we running?
if [ -f /.dockerenv ] || grep -qa docker /proc/1/cgroup 2>/dev/null; then
    RUNTIME="docker"
elif [ -n "$SINGULARITY_NAME" ] || [ -n "$APPTAINER_NAME" ]; then
    RUNTIME="singularity/apptainer ($SINGULARITY_NAME$APPTAINER_NAME)"
else
    RUNTIME="host"
fi
echo "Runtime:      $RUNTIME"
echo "Hostname:     $(hostname)  User: $(id -un) ($(id -u))"
echo "Settings:     ${TESTSUITE_SETTINGS_FILE:-settings.sh (default)}"

# Compilers actually resolved via PATH right now
echo "--- Compilers ---"
for var in CC CXX FTN FC TEST_CC TEST_CXX TEST_FTN; do
    val="${!var}"
    [ -z "$val" ] && continue
    resolved=$(command -v "$val" 2>/dev/null || echo "NOT FOUND")
    printf '  %-9s = %-10s -> %s\n' "$var" "$val" "$resolved"
done
if command -v "$CC" &>/dev/null; then
    echo "  \$CC version : $($CC --version 2>&1 | head -1)"
fi

# MPI wrappers
echo "--- MPI ---"
for var in TEST_CC_MPI TEST_CXX_MPI TEST_FTN_MPI TEST_RUN_CMD; do
    val="${!var}"
    [ -z "$val" ] && continue
    resolved=$(command -v "$val" 2>/dev/null || echo "NOT FOUND")
    printf '  %-13s = %-10s -> %s\n' "$var" "$val" "$resolved"
done

# Spack, if present
if command -v spack &>/dev/null; then
    echo "--- Spack ---"
    echo "  spack: $(command -v spack)  ($(spack --version 2>&1))"
    echo "  env:   $(spack env status 2>&1)"
fi

#echo "PATH: $PATH"
echo "================================="


$PYTHON_CMD ./test-all.py "$@"
