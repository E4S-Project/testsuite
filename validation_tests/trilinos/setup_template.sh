#!/bin/bash
if [ -n "$TRILINOS_ROOT" ]; then
	echo "Already set!"
	return
fi

if [ -f ../../../setup.sh ]; then
  . ../../../setup.sh 
fi 
. ../setup.sh
