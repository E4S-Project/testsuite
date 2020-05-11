#!/bin/bash

cd $SOLLVE
# We assume that a given LLVM module has been loaded earlier

OUTPUTFILE=results.json

make tidy
make CC=clang CXX=clang++ FC=f18 LOG=1 all
make report_json
cp $OUTPUTFILE $OLDPWD
cd $OLDPWD

COMPILED=`grep "Compiler result" $OUTPUTFILE | wc -l` 
SUCCESS=`grep "Compiler result" $OUTPUTFILE | grep "PASS" | wc -l`
echo "Compiled: " $SUCCESS "/" $COMPILED

EXECUTED=`grep "Runtime result" $OUTPUTFILE | wc -l`
 SUCCESS=`grep "Runtime result" $OUTPUTFILE | grep "PASS" | wc -l`
 echo "Executed: " $SUCCESS "/" $EXECUTED

