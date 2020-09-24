#!/bin/bash
CXX=c++
# set up script
if [ $# -ne 1 ]; then
    echo -e "Usage: $0 GINKGO_BUILD_DIRECTORY"
    exit 1
fi
BUILD_DIR=${GINKGO_ROOT}
#THIS_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" &>/dev/null && pwd )
APP=`basename ${THIS_DIR}`
#source ${THIS_DIR}/../build-setup.sh
LINK_FLAGS="-lginkgo -lginkgo_omp -lginkgo_cuda -lginkgo_reference -lginkgo_hip -Wl,-rpath,${BUILD_DIR}/lib"
# build
${CXX} -std=c++14 -o ./${APP} ./${APP}.cpp \
       -I${BUILD_DIR}/include -I${BUILD_DIR}/include/ginkgo\
       -L${BUILD_DIR} ${LINK_FLAGS}
