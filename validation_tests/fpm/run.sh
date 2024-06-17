#!/bin/bash
. ./setup.sh
cd hello_complex
fpm run --all
fpm test
 
