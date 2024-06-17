#!/bin/bash
. ./setup.sh
set -x
set -e

cd hello_complex
fpm build
