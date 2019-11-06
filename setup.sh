#This is necessary in every sub-script that loads spack packages.
command -v spack >/dev/null 2>&1 || { source /spack/share/spack/setup-env.sh; }
command -v spack >/dev/null 2>&1 || { echo "Failure: Spack not found. Exiting"; exit 1; }

oneSpackHash(){
	findOut="$(spack find -l $1)";
        if [ $? -ne 0 ] ; then
             echo "Package/Spec $1 not found." >&2
             exit 215;
        fi
	echo "/`echo "${findOut}" | tail -n1 | awk '{print $1;}'`" ;  
}
spackLoadUnique(){ 

        rArg=" -r "
	if [ ! -z "$2" ] && [ $2 = "nor" ]; then
		rArg=""
	fi
	spackHash="$(oneSpackHash $1)"
	ret_val=$?
	if [ $ret_val -ne 0 ] ; then
             exit 215;
        fi
	spack load ${rArg} ${spackHash} || { echo "Package/Spec $1 or dependency not found." >&2 ; exit 215; } ;
}

