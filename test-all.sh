#!/bin/bash 


if [ -n "$TERM" ];
then
bold=$(tput bold)$(tput setaf 1)
green=$(tput bold)$(tput setaf 2)
yellow=$(tput bold)$(tput setaf 3)
normal=$(tput sgr0);
fi
final_ret=0
ran_test=true
print_json=false
print_logs=false
basedir=validation_tests

    if [[ $# -gt 0 && -d $1 ]] ; then
       basedir=$1
     fi
while test $# -gt 0
do
    case "$1" in
        --json) print_json=true
            ;;
        --print-logs) print_logs=true
            ;;
        --settings) export TESTSUITE_SETTINGS_FILE=`readlink -f "$2"`
        shift
            ;;
        --help)
        echo "Usage:"
        echo "    ./test-all.sh [Test Directory (Optional. Must come first if used)] <--json> <--settings [settings file]>"
        echo "    --json: Print json output. Redirect to file manually if needed. e.g. ./test-all.sh --json > testout.json"
        echo "    --print-logs: Print contents of all clean/compiler/run logs to screen."
        echo "    --settings </path/to/some.settings.sh>: Use the specified settings.sh file to define compile and run options. Defaults to <testsuite>/settings.sh"
        echo "Examples:"
        echo "    ./test-all.sh #Run all tests in the <testsuite>/validation_tests directory"
        echo "    ./test-all.sh /path/to/test/directory #Run all tests in the specified directory"
        echo "    ./test-all.sh /path/to/test/directory --json --settings /path/to/some.settings.sh #Run all tests in the specified directory, print output as json, use some.settings.sh as settings file"
        exit 0
    esac
    shift
done

. ./setup.sh

if [ $print_json = true ]; then
                echo "["
fi

#  If $1 is a directory, run tests or recurse into it.
iterate_directories() {
    testdir=$1
    local _ret
    if [ -d $testdir ] ; then
        cd $testdir
	if [ $print_json != true ]; then
	
	
        	echo "==="
        	echo $testdir
	fi
        ran_test=true
        iterate_files
         _ret=$?
         if [ $_ret -ne 0 ] ; then
            final_ret=$( expr $final_ret + 1 )
         fi

	#if [ $print_json = true ]; then
	#	echo "}},"
	#fi

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

    if [ $print_json = true ]; then
        printf "{\"test\": \"$testdir\",  \"test_stages\": {"
    fi
    unset E4S_TEST_SETUP
    export SPACK_LOAD_RESULT=0
    source $cwd/setup.sh
    _ret=$SPACK_LOAD_RESULT
    export E4S_TEST_SETUP=1
            if [ $_ret -eq 215 ] ; then
             if [ $print_json = true ]; then
                         echo "\"missing\"}},"
             else
                 echo "Required Spack Packages ${yellow}Not Found${normal}" >&2
                 fi
             return $_ret
         fi
         if [ $_ret -ne 0 ] ; then

                 if [ $print_json = true ]; then
                 echo "\"fail\"}},"
             else
                 echo "Clean ${bold}failed${normal}" >&2
             fi
                 return $_ret
         fi


    if [ -e "$cwd/clean.sh" ] ; then
	    if [ $print_json = true ]; then
		    printf "\"clean\":"
	    else
            	echo "Cleaning $cwd"
    	fi
        ./clean.sh >& ./clean.log
        _ret=$?
        
        if [ $print_logs = true ]; then
             echo "---CLEANUP LOG---"
             cat ./clean.log
        fi
        if [ $_ret -eq 215 ] ; then
             if [ $print_json = true ]; then
		         echo "\"missing\"}},"
             else
                 echo "Required Spack Packages ${yellow}Not Found${normal}" >&2
	         fi
             return $_ret
         fi
         if [ $_ret -ne 0 ] ; then

   	         if [ $print_json = true ]; then
                 echo "\"fail\"}},"
             else
                 echo "Clean ${bold}failed${normal}" >&2
     	     fi
                 return $_ret
         fi
     fi
	 if [ $print_json = true ]; then
                     printf "\"pass\","
     fi
     if [ -e "$cwd/compile.sh" ] ; then
	     if [ $print_json = true ]; then
             printf "\"compile\":"
    	 else
             echo "Compiling $cwd"
         fi
            ./compile.sh >& ./compile.log
            _ret=$?
            if [ $print_logs = true ]; then
                 echo "---COMPILE LOG---"
                 cat ./compile.log
            fi

         if [ $_ret -eq 215 ] ; then
             if [ $print_json = true ]; then
                 echo "\"missing\"}},"
             else
                 echo "Required Spack Packages ${yellow}Not Found${normal}" >&2
	         fi
	         return $_ret
         fi
         if [ $_ret -ne 0 ] ; then
		 if [ $print_json = true ]; then
                     echo "\"fail\"}},"
             else

             echo "Compile ${bold}failed${normal}" >&2
     fi
             return $_ret
           fi

        fi
	if [ $print_json = true ]; then
                     printf "\"pass\","
         fi
	 if [ $print_json = true ]; then
            printf "\"run\":"
            else

        echo "Running $cwd"
	fi
        ./run.sh >& run.log
        _ret=$?
        if [ $print_logs = true ]; then
             echo "---RUN LOG---"
             cat ./run.log
        fi

           if [ $_ret -eq 215 ] ; then
	     if [ $print_json = true ]; then
                     echo "\"missing\"}},"
             else

             echo "Required Spack Packages ${yellow}Not Found${normal}" >&2
             fi
             return $_ret
           fi
           if [ $_ret -ne 0 ] ; then
		   if [ $print_json = true ]; then
                     echo "\"fail\"}},"
             else

             echo "Run ${bold}failed${normal}" >&2
     		fi
             return $_ret
           fi
	   if [ $print_json = true ]; then
                echo "\"pass\"}},"
          else
        
	   echo "${green}Success${normal}" >&2
	   fi
    else
        ran_test=false
    fi
}

#set -x

iterate_directories $basedir
if [ $print_json = true ]; then
                echo "]"
fi
exit $final_ret
