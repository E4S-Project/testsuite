#!/bin/bash
. ./setup.sh
set -x
set -e
swift-t fib.swift -n=7 
