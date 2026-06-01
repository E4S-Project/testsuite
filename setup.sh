#This is necessary in every sub-script that loads spack packages.
command -v spack >/dev/null 2>&1 || { source /spack/share/spack/setup-env.sh; }
command -v spack >/dev/null 2>&1 || { echo "Failure: Spack not found. Exiting"; exit 1; }
source ${SPACK_ROOT}/share/spack/setup-env.sh
#source ./settings.sh
#echo $BASH_SOURCE
#echo dirname $BASH_SOURCE
if [ -z ${TESTSUITE_SETTINGS_FILE+x} ]; then source `dirname $BASH_SOURCE`/settings.sh; else source $TESTSUITE_SETTINGS_FILE; fi


if ! command -v "$TEST_RUN_CMD" &> /dev/null; then
    echo "Error: The MPI command '$TEST_RUN_CMD' was not found. Check your settings.sh" >&2
    exit 1
fi

if ! $TEST_RUN hostname &> /dev/null; then
    echo "Error: The MPI command '$TEST_RUN' hostname failed to execute. Check your settings.sh" >&2
    exit 1
fi


#alias test_run='$TEST_RUN'
rArg="  "
dArg=" -dpl "
#dArg=" -pl "

spackTestRun(){
	#set -x
	testOut=$(script -q -e -c "spack test run /${1}") # 2>/dev/null)
	res=$?
	echo "--- $testOut ---"
	#echo $testOut | grep "No installed packages match spec"
	#grepRes=$?
	#echo $grepRes
	
	if [ $res -eq 0 ]; then
		testHash=$(echo "$testOut" | head -n 1 | awk '{print $NF}')
		echo TEST-HASH: $testHash
		spack test results -l $testHash
	fi

	return $res
}

spackHashFromName(){
	E4S_SPACK_TEST_HASH_NAME=${@^^}_HASH
	E4S_SPACK_TEST_HASH_NAME=`echo $E4S_SPACK_TEST_HASH_NAME | tr '-' '_'`
	echo ${!E4S_SPACK_TEST_HASH_NAME}
}

spackSetPackageRoot(){
 #       echo ${1}
        ARCH_IFS=$IFS
        IFS=' '
        PACK_ARRAY=(${1})
	IFS=$ARCH_IFS
#       echo ${PACK_ARRAY[0]}
#       echo ${PACK_ARRAY[1]}
#       echo ${PACK_ARRAY[2]}
        NAME_BLOB=${PACK_ARRAY[1]}
        SPAC_NAM=${NAME_BLOB%@*}
        SPAC_LOC=${PACK_ARRAY[2]} #`spack location -i ${1}`
        SPAC_NAM=${SPAC_NAM^^}
        SPAC_NAM=`echo $SPAC_NAM | tr '-' '_'`
	if [ $SPAC_NAM = BINUTILS ];then
		SPAC_NAM=BINUTILS_E4S
	fi
        #eval "${SPAC_NAM}_ROOT"
        ###BEWARE: Setting this value can 
        #echo $SPAC_NAM
        export ${SPAC_NAM}_ROOT=${SPAC_LOC}
        export ${SPAC_NAM}_HASH=${PACK_ARRAY[0]}

        export ${SPAC_NAM}_LIB_PATH=${SPAC_LOC}/lib64
        if [[ ! -d ${SPAC_LOC}/lib64 ]]; then
          export ${SPAC_NAM}_LIB_PATH=${SPAC_LOC}/lib
        fi
	LIBPATHVAR=${SPAC_NAM}_LIB_PATH
	export LD_LIBRARY_PATH=${!LIBPATHVAR}:$LD_LIBRARY_PATH
}

expinst=" -x "

spackGetUniqueExplicit(){
#set -x
	xhashes=`spack find $expinst --format {hash} $@ $TESTSUITE_VARIANT`
	ret_val=$?
	if [ $ret_val -ne 0 ] ; then
		#echo "get unique Returning 215!"
		export SPACK_LOAD_RESULT=215
		return 215
	fi
	echo $xhashes | awk '{print $1}'
#unset -x
}

spackLoadUnique(){
   if [[ ! -z $E4S_TEST_SETUP ]]; then
	   echo "Skipping load: Environment already setup"
	   return
   fi
   #SPACK_LOAD_RESULT=0
   
    uniquehash=$(spackGetUniqueExplicit "$@" 2>&1) #Errors go to stdout
    ret_val=$?

   if [ $ret_val -ne 0 ] ; then
        #echo "Returning 215!"
	echo "$uniquehash" 
        export SPACK_LOAD_RESULT=215
        return 215
   fi

   #spack load $rArg --first $@ $TESTSUITE_VARIANT
   spack load /${uniquehash}
   ret_val=$?
   #echo "Load return: $ret_val"
   if [ $ret_val -ne 0 ] ; then
	#echo "Returning 215!"
	export SPACK_LOAD_RESULT=215
        return 215
   fi

   #FIND_ARRAY1=($(spack find -l --loaded $@))  #`spack find -l --loaded $@`
   #HASHDEX=${#FIND_ARRAY1[@]}-2
   HASH=${uniquehash}   #${FIND_ARRAY1[HASHDEX]}
   
   echo "$@ $TESTSUITE_VARIANT: $HASH" >&1
   export E4S_TEST_HASH=$HASH
   #return 0
   ARCH_IFS=$IFS
   FIND_BLOB2=`spack find $dArg /$HASH`
   IFS=$'\n'
   FIND_ARRAY2=(${FIND_BLOB2})   #($($dArg /$HASH))
   IFS=$ARCH_IFS

   # Safely declare our regex in a variable to avoid bash parsing quirks.
   # This matches the Hash (1), the Indentation Spaces (2), and the Package Spec (3).
   line_regex='^([a-zA-Z0-9]+)( +)([^ ]+)'

   for((i=${#FIND_ARRAY2[@]}-1; i>=0; i--)); do
	  # set -x
        line="${FIND_ARRAY2[i]}"
        
        # Stop at the header
        if [[ "$line" == --*  ]]; then
                break
        fi

        # Extract the hash, indentation spaces, and package info 
        if [[ "$line" =~ $line_regex ]]; then
                spaces="${BASH_REMATCH[2]}"
                pkg_info="${BASH_REMATCH[3]}"
                pkg_name="${pkg_info%@*}" # Strip out the @version and +variants to get the raw name
                
                # Check dependency depth based on space length
                # Length 1 = Root package
                # Length 5 = 1st-tier dependencies
                # Length > 5 = 2nd-tier and beyond
                if (( ${#spaces} > 5 )); then
                        continue
                fi
                
                # Exclude compiler-related packages.
                # Note: We check that space length > 1, so if you ever explicitly 
                # run this harness targeting gcc or glibc directly, it won't skip it!
                if (( ${#spaces} > 1 )); then
                        case "$pkg_name" in
                                compiler-wrapper|gcc|gcc-runtime|glibc)
                                        continue
                                        ;;
                        esac
                fi
        fi

	#echo "Setting root for $line"
	#set +x
        # Pass the original, unmodified line into your existing parsing function
        spackSetPackageRoot "$line"
   done


 #  for((i=${#FIND_ARRAY2[@]}-1; i>=0; i--)); do
 #       #echo ${FIND_ARRAY2[i]}
 #       if [[ ${FIND_ARRAY2[i]} == --*  ]]; then
 #               break
 #       fi
 #       spackSetPackageRoot "${FIND_ARRAY2[i]}"
 #  done
   IFS=$ARCH_IFS
}

spackLoadUniqueNoX(){
  expinst=" "
  spackLoadUnique $@
  expinst=" -x "
}

spackLoadUniqueNoR(){
	#spack load $@
	rArg=" --only package "
	dArg=" -pl "
	spackLoadUnique $@
	_ret=$?
        rArg="  "
	dArg=" -dpl "
	return $_ret
}

