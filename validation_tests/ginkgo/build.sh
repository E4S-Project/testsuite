#!/bin/bash
CXX=$TEST_CXX #c++
# set up script
#if [ $# -ne 1 ]; then
#    echo -e "Usage: $0 GINKGO_BUILD_DIRECTORY"
#    exit 1
#fi
set -x
BUILD_DIR=${GINKGO_ROOT}
#THIS_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" &>/dev/null && pwd )
APP=$(basename $(pwd))
#source ${THIS_DIR}/../build-setup.sh
LINK_FLAGS="-lginkgo -lginkgo_device -lginkgo_omp -lginkgo_dpcpp -lginkgo_cuda -lginkgo_reference -lginkgo_hip -lpthread -Wl,-rpath,${GINKGO_LIB_PATH}"
# build
${CXX} -std=c++14 -o ./${APP} ./${APP}.cpp \
       -I${BUILD_DIR}/include -I${BUILD_DIR}/include/ginkgo\
       -L${GINKGO_LIB_PATH} ${LINK_FLAGS}
