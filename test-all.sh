#!/bin/bash 

final_ret=0
ran_test=true
print_json=false
export print_json=false
print_logs=false
export print_logs=false
basedir=validation_tests
export e4s_print_color=true
skip_to=""
testtime=$(date +"%Y-%m-%d_%T")

    if [[ $# -gt 0 && -d $1 ]] ; then
       basedir=$1
     fi
while test $# -gt 0
do
    if [[ -d $1 ]] ; then
       basedir=$1
    else

    case "$1" in
        --json) print_json=true; export print_json=true
            ;;
        --print-logs) print_logs=true; export print_logs=true
            ;;
        --settings) export TESTSUITE_SETTINGS_FILE=`readlink -f "$2"`
        shift
            ;;
	--color-off) export e4s_print_color=false
	    ;;
	--skip-to) skip_to="$2"
	shift
	    ;;
	--skip-if) skip_if="$2"
        shift
            ;;
        --test-only) test_only="$2"
        shift
            ;;
        --help)
        echo "Usage:"
        echo "    ./test-all.sh [Test Directory (Optional.)] <--json> <--settings [settings file]> <--color-off>"
        echo "    --json: Print json output. Redirect to file manually if needed. e.g. ./test-all.sh --json > testout.json"
        echo "    --print-logs: Print contents of all clean/compiler/run logs to screen."
        echo "    --settings </path/to/some.settings.sh>: Use the specified settings.sh file to define compile and run options. 
		Defaults to <testsuite>/settings.sh"
	echo "    --color-off: disable printing test results in color"
	echo "    --skip-to [test]: Start with the specified test, skipping over any listed earlier in lexical order."
	echo "    --test-only [\"list of tests\"]: Run only the tests named in the list."
	echo "    --skip-if [substring]: Bypass any test with the given substring in its base directory."
        echo "Examples:"
        echo "    ./test-all.sh #Run all tests in the <testsuite>/validation_tests directory"
        echo "    ./test-all.sh /path/to/test/directory #Run all tests in the specified directory"
        echo "    ./test-all.sh /path/to/test/directory --json --settings /path/to/some.settings.sh #Run all tests in the specified directory, 
		#print output as json, use some.settings.sh as settings file"
        exit 0
    esac
    fi
    shift
done

if [ $e4s_print_color = true -a  -n "$TERM" ];
then
bold=$(tput bold)$(tput setaf 1)
green=$(tput bold)$(tput setaf 2)
yellow=$(tput bold)$(tput setaf 3)
normal=$(tput sgr0);
fi

. ./setup.sh

if [ $print_json = true ]; then
                echo "["
fi

#  If $1 is a directory, run tests or recurse into it.
iterate_directories() {
    testdir=$1
    export testdir=$testdir
    local _ret
    if [ -d $testdir ] ; then
        cd $testdir
	if [ $print_json != true ]; then	
        	echo "==="
        	echo $testdir
	fi
        
    cwd=`pwd`	
    if [ -e "$cwd/run.sh" ] ; then
	#echo "Running in $cwd"
        local itout
	itout=$(~/testsuite/iterate_files.sh)
         _ret=$?
	 echo $itout
         if [ $_ret -ne 0 ] ; then
            final_ret=$( expr $final_ret + 1 )
         fi

	#if [ $print_json = true ]; then
	#	echo "}},"
	#fi

    else
	    #set -x
            for d in */ ; do
		if [ ! -z $skip_to ] && [[ $d < $skip_to ]]; then
			continue
		fi
		if [ ! -z $skip_if ] &&[[ `basename $d` == *"$skip_if"*  ]]; then
			continue
		fi
		if [ ! -z "$test_only" ] && echo "$test_only" | grep -vw -P "(?<![\w-])`basename $d`(?![\w-])" >/dev/null; then
			continue
		fi
		#echo $d
                iterate_directories $d
            done
        fi
        cd ..
    fi
}

#set -x

iterate_directories $basedir
if [ $print_json = true ]; then
                echo  "{}]"
fi
exit $final_ret
