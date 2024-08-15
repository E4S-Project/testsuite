#!/bin/bash
. ./setup.sh
set -e
set -x

which podman

e4s-alc create -f ./light_ubuntu@20.04-yaml.yaml 
