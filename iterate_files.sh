#!/bin/bash 

final_ret=0

if [ $e4s_print_color = true -a  -n "$TERM" ];
then
bold=$(tput bold)$(tput setaf 1)
green=$(tput bold)$(tput setaf 2)
yellow=$(tput bold)$(tput setaf 3)
normal=$(tput sgr0);
fi

# Check to see if this directory has clean, compile and run scripts.
# If so, set a flag so we don't recurse further.
cwd=$PWD 
_ret=0

if [ $print_json = true ]; then
	echo "{\"test\": \"$(basename "$testdir")\", \"test_stages\": {" >&2
else
    echo "===" >&1
    echo "$(basename "$testdir")" >&1
fi
unset E4S_TEST_SETUP
export SPACK_LOAD_RESULT=0
source $cwd/setup.sh 
_ret=$SPACK_LOAD_RESULT
export E4S_TEST_SETUP=1
E4S_LOG_SUFFIX="$(basename $testdir)"_"$E4S_TEST_HASH"_"$testtime".log
#echo $E4S_LOG_SUFFIX >&2
#echo "SPACK LOAD RESULT RETURN: $_ret" >&2
        if [ $_ret -eq 215 ] ; then
         if [ $print_json = true ]; then
                     echo "\"setup\":\"missing\"}}," >&2
         else
             echo "Required Spack Packages ${yellow}Not Found${normal}" >&1
             fi
         exit $_ret
     fi
     if [ $_ret -ne 0 ] ; then

             if [ $print_json = true ]; then
             echo "\"setup\":\"fail\"}}," >&2
         else
             echo "Setup ${bold}failed${normal}" >&1
         fi
             exit $_ret
     fi


if [ -e "$cwd/clean.sh" ] ; then
    if [ $print_json = true ]; then
        echo "\"clean\":" >&2
    else
            echo "Cleaning $testdir" >&1
        fi
    ./clean.sh >& ./clean-"$E4S_LOG_SUFFIX"
    _ret=$?
    
    if [ $print_logs = true ]; then
         echo "---CLEANUP LOG---" >&1
         cat ./clean-$E4S_LOG_SUFFIX >&1
    fi
    if [ $_ret -eq 215 ] ; then
         if [ $print_json = true ]; then
             echo "\"missing\"}}," >&2
         else
             echo "Required Spack Packages ${yellow}Not Found${normal}" >&1
      fi
         exit $_ret
     fi
     if [ $_ret -ne 0 ] ; then

         if [ $print_json = true ]; then
               echo "\"fail\"}}," >&2
             else
               echo "Clean ${bold}failed${normal}" >&1
             fi
             exit $_ret
      fi
 
 if [ $print_json = true ]; then
                 echo "\"pass\"," >&2
     fi
 fi

 if [ -e "$cwd/compile.sh" ] ; then
     if [ $print_json = true ]; then
         echo "\"compile\":" >&2
     else
         echo "Compiling $cwd" >&1
     fi
        ./compile.sh >& ./compile-$E4S_LOG_SUFFIX
        _ret=$?
        if [ $print_logs = true ]; then
             echo "---COMPILE LOG---" >&1
             cat ./compile-$E4S_LOG_SUFFIX >&1
        fi

     if [ $_ret -eq 215 ] ; then
         if [ $print_json = true ]; then
             echo "\"missing\"}}," >&2
         else
             echo "Required Spack Packages ${yellow}Not Found${normal}" >&1
         fi
         exit $_ret
     fi
     if [ $_ret -ne 0 ] ; then
     if [ $print_json = true ]; then
                 echo "\"fail\"}}," >&2
              else
                echo "Compile ${bold}failed${normal}" >&1
              fi
         exit $_ret
      fi
  if [ $print_json = true ]; then
                 echo "\"pass\"," >&2
      fi
  fi

  if [ $print_json = true ]; then
        echo "\"run\":" >&2
  else
    echo "Running $cwd" >&1
  fi
    ./run.sh >& run-$E4S_LOG_SUFFIX
    _ret=$?
    if [ $print_logs = true ]; then
         echo "---RUN LOG---"
         cat ./run-$E4S_LOG_SUFFIX >&1
    fi

       if [ $_ret -eq 215 ] ; then
     if [ $print_json = true ]; then
                 echo "\"missing\"}}," >&2
         else
         echo "Required Spack Packages ${yellow}Not Found${normal}" >&1
         fi
         exit $_ret
       fi
       if [ $_ret -ne 0 ] ; then
       if [ $print_json = true ]; then
                 echo "\"fail\"}}," >&2
               else
                 echo "Run ${bold}failed${normal}" >&1
           fi
         exit $_ret
       fi
   if [ $print_json = true ]; then
            echo "\"pass\"}}, " >&2
      else
    
   echo "${green}Success${normal}" >&1
   fi


