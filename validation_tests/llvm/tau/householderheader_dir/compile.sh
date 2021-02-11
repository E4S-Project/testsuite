#!/bin/bash

. ./setup.sh

SOURCES="screen.cpp main.cpp"

test::compile "functions_CXX_header_implemented.txt" "householderheader" "$SOURCES"
