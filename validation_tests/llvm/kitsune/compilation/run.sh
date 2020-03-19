#!/bin/bash

# The goal here is to see if the compiler recognizes the syntax

clang -fsyntax-only -std=c99 interleaved_cilk.c -ftapir=cilk
clang++ -fsyntax-only forall2.cpp -ftapir=serial
clang -fsyntax-only basic_openmp.c -ftapir=openmp
