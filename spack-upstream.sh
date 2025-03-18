#!/bin/bash

show_help() {
  cat <<EOF
Usage: $0 -u <upstream_path> -l <local_parent_path>

This script clones a new local Spack installation and configures it to use an
upstream Spack installation.

Options:
  -u, --upstream <path>   Path to the upstream Spack installation. Defaults to
			  SPACK_ROOT.
  -l, --local <path>      Path to the parent directory for the new local Spack
                          installation which will be named 'spack-downstream'
  -d, --develop           Use develop version of spack (do not match upstream
			  version).
  -h, --help              Display this help message.

Notes:
  - To set an upstream for an *existing* local Spack, use the following
    command after initializing your Spack install:
      spack config --scope site add "upstreams:e4s:install_tree:\$UPSTREAM_SPACK_ROOT/opt/spack"
  - This script will checkout the spack commit matching the upstream spack if possible

Examples:

- Specify upstream Spack and local destination directory:
  $0 -u /path/to/upstream/spack -l /path/to/parent/of/new/local/spack

- Create a downstream Spack in the current directory, from an active upstream Spack:
  $0 -l .
EOF
  exit "$1"
}

if [[ $# -eq 0 ]]; then
  show_help 0
fi

upstream_spack=""
local_parent_path=""
match_version=1

while [[ $# -gt 0 ]]; do
  case "$1" in
    -u|--upstream)
      upstream_spack="$2"
      shift 2
      ;;
    -l|--local)
      local_parent_path="$2"
      shift 2
      ;;
    -h|--help)
      show_help 0
      ;;
    -d|--develop)
      match_version=0
    *)
      echo "Error: Unknown option '$1'"
      show_help 1
      ;;
  esac
done

rundir=`pwd`

#Determine Upstream Spack Path
if [[ -n "$upstream_spack" ]]; then
  # Upstream Spack path is provided by the user via --upstream
  : # Do nothing, already set
elif [[ -n "$SPACK_ROOT" ]]; then
  # Upstream Spack is loaded and --upstream is not specified
  upstream_spack="$SPACK_ROOT"
fi

if [[ ! -d "$upstream_spack" || ! -f "$upstream_spack/share/spack/setup-env.sh" ]]; then
  echo "Error: Invalid upstream Spack path: $upstream_spack"
  show_help 1
fi

# Handle Local Spack Parent Path (Create if Necessary)
if [[ -z "$local_parent_path" ]]; then
  echo "Error: Please specify the parent directory for the local Spack using --local."
  show_help 1
fi

mkdir -p "$local_parent_path"

# Set the actual local Spack path (where we will clone to)
local_spack="$local_parent_path/spack-downstream"

if [[ -d "$local_spack" && -f "$local_spack/share/spack/setup-env.sh" ]]; then
  echo "Error: '$local_spack' already contains a Spack installation."
  show_help 1
fi

# Clone Local Spack
echo "Cloning latest spack to $local_spack."
if ! git clone --recursive https://github.com/spack/spack "$local_spack"; then
  echo "Error: Failed to clone Spack to $local_spack"
  show_help 1
fi

if [[ $match_version -eq 1 &&  -d "$upstream_spack/.git" ]]; then
  # Checkout upstream's commit
  upstream_commit=$(cd "$upstream_spack" && git rev-parse HEAD)
  cd "$rundir"
  cd "$local_spack"
  if ! git checkout "$upstream_commit"; then
    echo "Warning: Could not checkout commit $upstream_commit from upstream Spack. Using latest from 'develop' branch instead."
  fi
fi

  # Activate local Spack
  cd "$rundir"
  if ! source "$local_spack/share/spack/setup-env.sh"; then
    echo "Failed to activate spack at $local_spack"
    exit 1
  fi

  # Enable upstreaming using spack config
  echo "Enabling upstreaming from $local_spack to $upstream_spack..."
  if ! spack config --scope site add "upstreams:e4s:install_tree:$upstream_spack/opt/spack"; then
    echo "Error: Failed to configure upstreaming. Check the Spack configuration." >&2
    exit 1
  fi

  # Copy over the upstream compilers.yaml
  cp $SPACK_ROOT/etc/spack/compilers.yaml{,.bak} >/dev/null 2>&1 || true

  if [[ -f $upstream_spack/etc/spack/compilers.yaml ]] ; then
    cp $upstream_spack/etc/spack/compilers.yaml $SPACK_ROOT/etc/spack/
  else
    echo warning: compilers.yaml not found in upstream $upstream_spack/etc/spack/compilers.yaml
  fi

  echo "Upstreaming enabled successfully! To activate run 'source $SPACK_ROOT/share/spack/setup-env.sh'"

exit 0
