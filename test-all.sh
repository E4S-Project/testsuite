#!/bin/bash 

ran_test=true

#  If $1 is a directory, run tests or recurse into it.
iterate_directories() {
    testdir=$1
    if [ -d $testdir ] ; then
        cd $testdir
        echo $testdir
        ran_test=true
        iterate_files
        if [ "$ran_test" == "false" ] ; then
            for d in * ; do
                iterate_directories $d
            done
        fi
        cd ..
    fi
}

# Check to see if this directory has clean, compile and run scripts.
# If so, set a flag so we don't recurse further.
iterate_files() {
    cwd=`pwd`
    if [ -e "$cwd/run.sh" ] ; then
        if [ -e "$cwd/clean.sh" ] ; then
            echo "Cleaning $cwd"
            ./clean.sh >& ./clean.log
        fi
        if [ -e "$cwd/compile.sh" ] ; then
            echo "Compiling $cwd"
            ./compile.sh >& ./compile.log
        fi
        echo "Running $cwd"
        ./run.sh >& run.log
    else
        ran_test=false
    fi
}

#set -x
iterate_directories validation_tests
