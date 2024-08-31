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
cwd=`pwd` 
_ret=0

if [ $print_json = true ]; then
    printf "{\"test\": \"$testdir\",  \"test_stages\": {"
fi
unset E4S_TEST_SETUP
export SPACK_LOAD_RESULT=0
source $cwd/setup.sh 
_ret=$SPACK_LOAD_RESULT
export E4S_TEST_SETUP=1
E4S_LOG_SUFFIX="$(basename $cwd)"_"$E4S_TEST_HASH"_"$testtime".log
#echo $E4S_LOG_SUFFIX >&2
#echo "SPACK LOAD RESULT RETURN: $_ret" >&2
        if [ $_ret -eq 215 ] ; then
         if [ $print_json = true ]; then
                     echo "\"setup\":\"missing\"}},"
         else
             echo "Required Spack Packages ${yellow}Not Found${normal}" >&2
             fi
         exit $_ret
     fi
     if [ $_ret -ne 0 ] ; then

             if [ $print_json = true ]; then
             echo "\"setup\":\"fail\"}},"
         else
             echo "Setup ${bold}failed${normal}" >&2
         fi
             exit $_ret
     fi


if [ -e "$cwd/clean.sh" ] ; then
    if [ $print_json = true ]; then
        printf "\"clean\":"
    else
            echo "Cleaning $cwd" >&2
        fi
    ./clean.sh >& ./clean-"$E4S_LOG_SUFFIX"
    _ret=$?
    
    if [ $print_logs = true ]; then
         echo "---CLEANUP LOG---" >&2
         cat ./clean-$E4S_LOG_SUFFIX >&2
    fi
    if [ $_ret -eq 215 ] ; then
         if [ $print_json = true ]; then
             echo "\"missing\"}},"
         else
             echo "Required Spack Packages ${yellow}Not Found${normal}" >&2
      fi
         exit $_ret
     fi
     if [ $_ret -ne 0 ] ; then

         if [ $print_json = true ]; then
               echo "\"fail\"}},"
             else
               echo "Clean ${bold}failed${normal}" >&2
             fi
             exit $_ret
      fi
 
 if [ $print_json = true ]; then
                 printf "\"pass\","
     fi
 fi

 if [ -e "$cwd/compile.sh" ] ; then
     if [ $print_json = true ]; then
         printf "\"compile\":"
     else
         echo "Compiling $cwd" >&2
     fi
        ./compile.sh >& ./compile-$E4S_LOG_SUFFIX
        _ret=$?
        if [ $print_logs = true ]; then
             echo "---COMPILE LOG---" >&2
             cat ./compile-$E4S_LOG_SUFFIX >&2
        fi

     if [ $_ret -eq 215 ] ; then
         if [ $print_json = true ]; then
             echo "\"missing\"}},"
         else
             echo "Required Spack Packages ${yellow}Not Found${normal}" >&2
         fi
         exit $_ret
     fi
     if [ $_ret -ne 0 ] ; then
     if [ $print_json = true ]; then
                 echo "\"fail\"}},"
              else
                echo "Compile ${bold}failed${normal}" >&2
              fi
         exit $_ret
      fi
  if [ $print_json = true ]; then
                 printf "\"pass\","
      fi
  fi

  if [ $print_json = true ]; then
        printf "\"run\":"
  else
    echo "Running $cwd" >&2
  fi
    ./run.sh >& run-$E4S_LOG_SUFFIX
    _ret=$?
    if [ $print_logs = true ]; then
         echo "---RUN LOG---"
         cat ./run-$E4S_LOG_SUFFIX >&2
    fi

       if [ $_ret -eq 215 ] ; then
     if [ $print_json = true ]; then
                 echo "\"missing\"}},"
         else
         echo "Required Spack Packages ${yellow}Not Found${normal}" >&2
         fi
         exit $_ret
       fi
       if [ $_ret -ne 0 ] ; then
       if [ $print_json = true ]; then
                 printf "\"fail\"}},"
               else
                  echo "Run ${bold}failed${normal}" >&2
           fi
         exit $_ret
       fi
   if [ $print_json = true ]; then
            printf "\"pass\"}},"
      else
    
   echo "${green}Success${normal}" >&2
   fi


