#!/bin/bash
. ./setup.sh
set -x
set -e

${TEST_CXX} ./boost-thread.cpp -o boost-thread -I$BOOST_ROOT/include -L$BOOST_ROOT/lib -lboost_thread -lboost_chrono
