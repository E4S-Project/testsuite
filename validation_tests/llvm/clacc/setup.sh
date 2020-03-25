#!/bin/bash

module load llvm/clacc/git
ulimit -s unlimited  # necessary for the potentially large VLA in householder2
