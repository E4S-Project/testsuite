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
source "$settings_file"
source setup.sh


check_mpi_runtime()
{
    local timeout_seconds="${MPI_SANITY_TIMEOUT:-30}"
    local probe_ranks="${MPI_SANITY_RANKS:-${TEST_RUN_PROCARG:-2}}"
    local tmpdir
    local probe_source
    local probe_binary
    local probe_output
    local status
    local node
    local node_state
    local bad_nodes=""

    echo "=== MPI Runtime Sanity Check ==="

    if [[ -z "${TEST_RUN_CMD:-}" ]]; then
        echo "Error: TEST_RUN_CMD is not set; cannot run MPI tests." >&2
        return 1
    fi

    if ! command -v "$TEST_RUN_CMD" >/dev/null 2>&1; then
        echo "Error: MPI launcher '$TEST_RUN_CMD' was not found in PATH." >&2
        return 1
    fi

    if [[ -z "${TEST_RUN_PROCFLAG:-}" ]]; then
        echo "Error: TEST_RUN_PROCFLAG is not set." >&2
        return 1
    fi

    if ! [[ "$probe_ranks" =~ ^[1-9][0-9]*$ ]]; then
        echo "Error: Invalid MPI sanity-check rank count: '$probe_ranks'." >&2
        return 1
    fi

    if ! command -v timeout >/dev/null 2>&1; then
        echo "Error: GNU 'timeout' is required for the MPI sanity check." >&2
        echo "Without it, a broken MPI launch could hang the test suite." >&2
        return 1
    fi

    #
    # If this is a Slurm allocation, detect nodes Slurm already knows are bad.
    # Do not reject DRAIN/DRAINING alone: an existing allocation may remain
    # usable while the node is being prevented from receiving future work.
    #
    if [[ -n "${SLURM_JOB_ID:-}" ]]; then
        if ! command -v scontrol >/dev/null 2>&1 ||
           ! command -v sinfo >/dev/null 2>&1; then
            echo "Error: SLURM_JOB_ID=$SLURM_JOB_ID is set, but scontrol or sinfo is unavailable." >&2
            echo "Cannot verify the health of the allocated Slurm nodes." >&2
            return 1
        fi

        echo "Checking nodes allocated to Slurm job $SLURM_JOB_ID..."

        while IFS= read -r node; do
            [[ -z "$node" ]] && continue

            node_state=$(
                sinfo -h -n "$node" -o '%T' 2>/dev/null |
                head -n 1 |
                tr '[:lower:]' '[:upper:]' |
                tr -d '*~#$@+'
            )

            if [[ -z "$node_state" ]]; then
                bad_nodes+="  $node: state could not be obtained from sinfo"$'\n'
                continue
            fi

            case "$node_state" in
                DOWN*|FAIL*|FAILING*|NO_RESPOND*|UNKNOWN*)
                    bad_nodes+="  $node: $node_state"$'\n'
                    ;;
            esac
        done < <(scontrol show hostnames "${SLURM_JOB_NODELIST:?SLURM_JOB_NODELIST is not set}")

        if [[ -n "$bad_nodes" ]]; then
            echo "Error: Slurm reports unusable nodes in the current allocation:" >&2
            printf '%s' "$bad_nodes" >&2
            echo >&2
            echo "MPI launch is likely to fail or hang on this allocation." >&2
            echo "Cancel/requeue Slurm job $SLURM_JOB_ID or obtain a healthy allocation." >&2
            echo "Additional Slurm diagnostics:" >&2
            sinfo -R >&2 || true
            return 1
        fi
    fi

    if [[ -z "${TEST_CC_MPI:-}" ]]; then
        echo "Error: TEST_CC_MPI is not set; cannot build the MPI sanity probe." >&2
        return 1
    fi

    if ! command -v "$TEST_CC_MPI" >/dev/null 2>&1; then
        echo "Error: MPI C compiler '$TEST_CC_MPI' was not found in PATH." >&2
        return 1
    fi

    tmpdir=$(mktemp -d "${TMPDIR:-/tmp}/mpi-sanity.XXXXXXXX") || {
        echo "Error: Could not create a temporary directory for the MPI sanity check." >&2
        return 1
    }

    probe_source="$tmpdir/mpi-sanity.c"
    probe_binary="$tmpdir/mpi-sanity"
    probe_output="$tmpdir/mpi-sanity.out"

    cat >"$probe_source" <<'EOF'
#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    int rank;
    int size;
    int initialized_count = 1;
    int total_initialized = 0;

    if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
        fprintf(stderr, "MPI_Init failed\n");
        return 10;
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (MPI_Allreduce(&initialized_count, &total_initialized, 1,
                      MPI_INT, MPI_SUM, MPI_COMM_WORLD) != MPI_SUCCESS) {
        fprintf(stderr, "MPI_Allreduce failed on rank %d\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 11);
        return 11;
    }

    if (rank == 0) {
        printf("MPI_SANITY_OK ranks=%d initialized=%d\n",
               size, total_initialized);
        fflush(stdout);
    }

    MPI_Finalize();
    return total_initialized == size ? 0 : 12;
}
EOF

    if ! "$TEST_CC_MPI" "$probe_source" -o "$probe_binary" >"$probe_output" 2>&1; then
        echo "Error: Failed to compile the MPI sanity-check program." >&2
        echo "Compiler: $TEST_CC_MPI" >&2
        cat "$probe_output" >&2
        rm -rf "$tmpdir"
        return 1
    fi

    echo "Testing: $TEST_RUN_CMD $TEST_RUN_PROCFLAG $probe_ranks <MPI probe>"
    echo "Timeout: ${timeout_seconds}s"

    timeout --signal=TERM --kill-after=5s "${timeout_seconds}s" \
        "$TEST_RUN_CMD" \
        "$TEST_RUN_PROCFLAG" "$probe_ranks" \
        "$probe_binary" >"$probe_output" 2>&1
    status=$?

    if [[ $status -eq 124 || $status -eq 137 ]]; then
        echo "Error: MPI failed to complete within ${timeout_seconds} seconds." >&2
        echo "The MPI launcher, allocated nodes, PMI/PMIx integration, or remote process startup is unresponsive." >&2

        if [[ -n "${SLURM_JOB_ID:-}" ]]; then
            echo "Slurm job:  $SLURM_JOB_ID" >&2
            echo "Node list:  ${SLURM_JOB_NODELIST:-unknown}" >&2
            echo "Try: sinfo -R" >&2
            echo "Try: scontrol show job $SLURM_JOB_ID" >&2
        fi

        if [[ -s "$probe_output" ]]; then
            echo "--- MPI launcher output ---" >&2
            cat "$probe_output" >&2
            echo "---------------------------" >&2
        fi

        rm -rf "$tmpdir"
        return 1
    fi

    if [[ $status -ne 0 ]]; then
        echo "Error: MPI sanity check failed with exit status $status." >&2
        echo "Command: $TEST_RUN_CMD $TEST_RUN_PROCFLAG $probe_ranks $probe_binary" >&2

        if [[ -s "$probe_output" ]]; then
            echo "--- MPI launcher output ---" >&2
            cat "$probe_output" >&2
            echo "---------------------------" >&2
        fi

        rm -rf "$tmpdir"
        return 1
    fi

    if ! grep -q "^MPI_SANITY_OK ranks=$probe_ranks initialized=$probe_ranks$" "$probe_output"; then
        echo "Error: MPI launcher exited successfully, but the probe did not report all expected ranks." >&2
        echo "Expected $probe_ranks initialized ranks." >&2
        echo "--- MPI launcher output ---" >&2
        cat "$probe_output" >&2
        echo "---------------------------" >&2
        rm -rf "$tmpdir"
        return 1
    fi

    cat "$probe_output"
    rm -rf "$tmpdir"

    echo "MPI runtime sanity check passed."
    echo "================================"
}


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
# Spack check
if ! command -v spack &>/dev/null; then
    echo "Error: 'spack' command not found. Aborting test." >&2
    return 1 2>/dev/null || exit 1
    echo "================================="
fi
    echo "--- Spack ---"
    echo "  spack: $(command -v spack)  ($(spack --version 2>&1))"
    echo "  env:   $(spack env status 2>&1)"

#echo "PATH: $PATH"
echo "================================="

check_mpi_runtime || exit 1

"$PYTHON_CMD" ./test-all.py "$@"
