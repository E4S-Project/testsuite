#!/bin/bash

THISDIR=$(basename "$PWD")

if ! python3 -c "import importlib.util, sys; sys.exit(0 if importlib.util.find_spec('cv2') else 1)"; then
    echo "CV2 not installed."
    export SPACK_LOAD_RESULT=215
else
    echo "Continuing to CV2 test"
fi
