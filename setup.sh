#This is necessary in every sub-script that loads spack packages.
command -v spack >/dev/null 2>&1 || { source /spack/share/spack/setup-env.sh; }
command -v spack >/dev/null 2>&1 || { echo "Failure: Spack not found. Exiting"; exit 1; }
source ${SPACK_ROOT}/share/spack/setup-env.sh
#source ./settings.sh
#echo $BASH_SOURCE
#echo dirname $BASH_SOURCE
if [ -z ${TESTSUITE_SETTINGS_FILE+x} ]; then source `dirname $BASH_SOURCE`/settings.sh; else source $TESTSUITE_SETTINGS_FILE; fi


#alias test_run='$TEST_RUN'
rArg=" -r "
dArg=" -dpl "
oneSpackHash(){
	findOut="$(spack find -l $@)";
        if [ $? -ne 0 ] ; then
             echo "Package/Spec $@ not found." >&2
             return 215;
        fi
	echo "/`echo "${findOut}" | tail -n1 | awk '{print $1;}'`" ;  
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
        #eval "${SPAC_NAM}_ROOT"
        ###BEWARE: Setting this value can 
        #echo $SPAC_NAM
        export ${SPAC_NAM}_ROOT=${SPAC_LOC}
        export ${SPAC_NAM}_HASH=${PACK_ARRAY[0]}

        export ${SPAC_NAM}_LIB_PATH=${SPAC_LOC}/lib

        if [[ ! -d ${SPAC_LOC}/lib ]]; then
          export ${SPAC_NAM}_LIB_PATH=${SPAC_LOC}/lib64
        fi
}

spackLoadUnique(){
   if [[ ! -z $E4S_TEST_SETUP ]]; then
	   echo "Skipping load: Environment already setup"
	   return
   fi
   SPACK_LOAD_RESULT=0
   spack load $rArg --first $@
   ret_val=$?
   #echo "Load return: $ret_val"
   if [ $ret_val -ne 0 ] ; then
	#echo "Returning 215!"
	export SPACK_LOAD_RESULT=215
        return 215
   fi

   FIND_ARRAY1=($(spack find -l --loaded $@))  #`spack find -l --loaded $@`
   HASHDEX=${#FIND_ARRAY1[@]}-2
   HASH=${FIND_ARRAY1[HASHDEX]}
   echo $HASH
   ARCH_IFS=$IFS
   FIND_BLOB2=`spack find $dArg /$HASH`
   IFS=$'\n'
   FIND_ARRAY2=(${FIND_BLOB2})   #($($dArg /$HASH))
   IFS=$ARCH_IFS
   for((i=${#FIND_ARRAY2[@]}-1; i>=0; i--)); do
        #echo ${FIND_ARRAY2[i]}
        if [[ ${FIND_ARRAY2[i]} == --*  ]]; then
                break
        fi
        spackSetPackageRoot "${FIND_ARRAY2[i]}"
   done
   IFS=$ARCH_IFS
}

spackLoadUniqueNoR(){
	#spack load $@
	rArg=" --only package "
	dArg=" -pl "
	spackLoadUnique $@
	_ret=$?
        rArg=" -r "
	dArg=" -dpl "
	return $_ret
}
