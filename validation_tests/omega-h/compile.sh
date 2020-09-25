#!/bin/bash
. ./setup.sh
mpicxx -o castle main.cpp -lomega_h
