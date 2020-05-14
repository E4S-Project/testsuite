#!/bin/bash

cd $SOLLVE
make tidy
cd $OLDPWD

module unload sollve/git
rm -f results.json


