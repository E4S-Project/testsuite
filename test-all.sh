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
print_json=false

#  If $1 is a directory, run tests or recurse into it.
iterate_directories() {
    testdir=$1
    if [ -d $testdir ] ; then
        cd $testdir
	if [ $print_json != true ]; then
	
	
        	echo "==="
        	echo $testdir
	fi
        ran_test=true
        iterate_files

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

        if [ -e "$cwd/clean.sh" ] ; then
	    if [ $print_json = true ]; then
		    printf "\"clean\":"
	    else
            	echo "Cleaning $cwd"
    	    fi
            ./clean.sh >& ./clean.log
            _ret=$?
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

basedir=validation_tests
    if [[ $# -gt 0 && -d $1 ]] ; then
       basedir=$1
     fi
while test $# -gt 0
do
    case "$1" in
        --json) print_json=true
            ;;
    esac
    shift
done
if [ $print_json = true ]; then
                echo "["
fi

iterate_directories $basedir
if [ $print_json = true ]; then
                echo "]"
fi

