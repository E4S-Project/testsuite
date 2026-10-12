#!/bin/bash

. ./setup.sh
set -e
set -x

${TEST_RUN_SEQ} ./ascent_render_example
${TEST_RUN_SEQ} ./ascent_render_cinema_example

