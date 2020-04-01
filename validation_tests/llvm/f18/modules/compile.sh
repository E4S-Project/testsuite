#!/bin/bash

f18 -module-suffix .f18.mod -c mod1.f90
f18 -module-suffix .f18.mod -o use-mod1 use-mod1.f90

