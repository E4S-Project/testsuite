#!/bin/bash

. ../testfunctions.sh

SOURCES="screen.cpp main.cpp"

# Generate .symbol database if needed
symbols::analysis "$SOURCES" 
