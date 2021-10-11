#!/bin/bash

. ./setup.sh
export TSAN_OPTIONS="no_huge_pages_for_shadow=0"

./parallel-simple
