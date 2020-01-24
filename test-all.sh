#!/bin/bash 

. ./setup.sh
if [ -n "$TERM" ];
then
bold=$(tput bold)$(tput setaf 1)
green=$(tput bold)$(tput setaf 2)
yellow=$(tput bold)$(tput setaf 3)
normal=$(tput sgr0);
fi
ran_test=true

#  If $1 is a directory, run tests or recurse into it.
iterate_directories() {
    testdir=$1
    if [ -d $testdir ] ; then
        cd $testdir
        echo "==="
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
    local _ret
    if [ -e "$cwd/run.sh" ] ; then
        if [ -e "$cwd/clean.sh" ] ; then
            echo "Cleaning $cwd"
            ./clean.sh >& ./clean.log
            _ret=$?
           if [ $_ret -eq 215 ] ; then
             echo "Required Spack Packages ${yellow}Not Found${normal}" >&2
             return $_ret
           fi
           if [ $_ret -ne 0 ] ; then
             echo "Clean ${bold}failed${normal}" >&2
             return $_ret
           fi
        fi
        if [ -e "$cwd/compile.sh" ] ; then
            echo "Compiling $cwd"
            ./compile.sh >& ./compile.log
            _ret=$?
           if [ $_ret -eq 215 ] ; then
             echo "Required Spack Packages ${yellow}Not Found${normal}" >&2
             return $_ret
           fi
           if [ $_ret -ne 0 ] ; then
             echo "Compile ${bold}failed${normal}" >&2
             return $_ret
           fi

        fi
        echo "Running $cwd"
        ./run.sh >& run.log
        _ret=$?
           if [ $_ret -eq 215 ] ; then
             echo "Required Spack Packages ${yellow}Not Found${normal}" >&2
             return $_ret
           fi
           if [ $_ret -ne 0 ] ; then
             echo "Run ${bold}failed${normal}" >&2
             return $_ret
           fi
	   echo "${green}Success${normal}" >&2

    else
        ran_test=false
    fi
}

#set -x

basedir=validation_tests
    if [[ $# -gt 0 && -d $1 ]] ; then
       basedir=$1
     fi
iterate_directories $basedir
