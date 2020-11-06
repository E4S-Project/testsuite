#!/bin/bash

. ./setup.sh

$TEST_CC -o hello-world hello-world.c -laml
