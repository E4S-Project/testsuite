if [ -z ${TESTSUITE_SETTINGS_FILE+x} ]; then source `dirname $BASH_SOURCE`/settings.sh; else source $TESTSUITE_SETTINGS_FILE; fi

if [ "$E4S_TEST_SETUP_MODE" == "module" ]; then

    # ---------------------------------------------------------
    # MODULE-BASED SETUP
    # ---------------------------------------------------------

    spackLoadUnique() {
		if grep -q "spackTestRun" ./run.sh ; then
		    echo "Module setup error: Cannot run internal spack test for: $*" >&2
            export SPACK_LOAD_RESULT=216
            return 216
		fi
	
	
        # Clear stale state from any previous call so a prior failure/arch
        # detection doesn't leak into this one.
        #unset SPACK_LOAD_RESULT SPACK_CUDA_ARCH SPACK_ROCM_ARCH

        # 1. Add spaces before spack flags to split concatenated ones (e.g., ~cuda~rocm -> ~cuda ~rocm)
        local all_args=$(echo "$*" | sed -e 's/\(~\|+\|@\|%\)/ \1/g')

        # Extract base package name from the first word
        local pkg_base=$(echo "$all_args" | awk '{print $1}' | sed 's/[@+~%^].*//')

		# 2. Get all available modules for this base package (strip tags like <L> or (default))
        local mods=$(module -t avail "$pkg_base" 2>&1 | grep "^${pkg_base}/" | sed 's/[ <(].*//')

        # 3. Filter the module list based on provided spack variants
        for arg in $all_args; do
            # Skip the package name itself
            if [[ "$arg" == "$pkg_base" ]]; then continue; fi

            if [[ "$arg" == "~"* ]]; then
                # ~variant -> strictly exclude from module name (case-insensitive,
                # since cuda/rocm tags can carry an arch suffix like cuda90/rocm60)
                mods=$(echo "$mods" | grep -vi "${arg:1}")

            elif [[ "$arg" == "+"* ]]; then
                local variant="${arg:1}"
                case "$variant" in
                    cuda|rocm)
                        # cuda/rocm are always encoded in the module name (possibly
                        # with an arch suffix, e.g. cuda90) -- this is a hard
                        # requirement, not a soft preference. If nothing matches,
                        # mods legitimately becomes empty and the module is
                        # reported as not found below.
                        mods=$(echo "$mods" | grep -i "$variant")
                        ;;
                    *)
                        # other +variant -> soft include (prefer if module exists,
                        # otherwise ignore), since most variants aren't encoded
                        # in the module name at all.
                        local filtered=$(echo "$mods" | grep "$variant")
                        if [ -n "$filtered" ]; then mods="$filtered"; fi
                        ;;
                esac

            elif [[ "$arg" == "@"* ]]; then
                # @version -> ignore ranges with ':', otherwise soft include
                local v_stripped="${arg:1}"
                if [[ "$v_stripped" != *":"* ]]; then
                    local filtered=$(echo "$mods" | grep "${v_stripped}")
                    if [ -n "$filtered" ]; then mods="$filtered"; fi
                fi

            elif [[ "$arg" == "%"* ]]; then
                # %compiler -> soft include
                local filtered=$(echo "$mods" | grep "${arg:1}")
                if [ -n "$filtered" ]; then mods="$filtered"; fi
            fi
        done

        # 4. Pick the first module that survived the filters
        local selected_mod=$(echo "$mods" | head -n 1)
        if [ -z "$selected_mod" ]; then
            # Mirror the spack-path convention: a package/variant combo that
            # doesn't exist is a 215, not a hard error, so the driver treats it
            # as a skip rather than a failed build.
            echo "Module setup error: No matching module found for: $*" >&2
            export SPACK_LOAD_RESULT=215
            return 215
        fi

        # Pull an arch suffix off cuda/rocm tags in the module name, e.g.
        # "ginkgo/1.11.0-cuda90-openmp" -> SPACK_CUDA_ARCH=90. Only set when a
        # numeric suffix is actually present; a bare "cuda"/"rocm" tag with no
        # arch encoded leaves the corresponding var unset.
        if [[ "$selected_mod" =~ [Cc][Uu][Dd][Aa]([0-9]+) ]]; then
            export SPACK_CUDA_ARCH="${BASH_REMATCH[1]}"
        fi
        if [[ "$selected_mod" =~ [Rr][Oo][Cc][Mm]([0-9]+) ]]; then
            export SPACK_ROCM_ARCH="${BASH_REMATCH[1]}"
        fi

        echo "Loading module: $selected_mod"

        # Snapshot LD_LIBRARY_PATH *before* loading. `module load` will recursively
        # autoload every transitive dependency of $selected_mod, and each of those
        # modulefiles may itself prepend-path its own lib dir onto LD_LIBRARY_PATH.
        # That's what was causing every loaded module's lib dir to leak in. We don't
        # try to stop lmod/environment-modules from doing that (PATH/CMAKE_PREFIX_PATH
        # etc. still need the full transitive load) -- instead we rebuild
        # LD_LIBRARY_PATH from scratch afterward using only the packages we actually
        # want, mirroring the spack-path's depth-limited spackSetPackageRoot behavior.
        local PRE_LOAD_LD_LIBRARY_PATH="$LD_LIBRARY_PATH"

        if ! module load "$selected_mod"; then
            echo "Module setup error: Failed to load module: $selected_mod" >&2
            export SPACK_LOAD_RESULT=215
            return 215
        fi

        # 5. Figure out which packages count as "close enough" to matter for
        #    LD_LIBRARY_PATH: the root package itself, plus its *direct* (tier-1)
        #    dependencies, excluding compiler-related packages. `module show` only
        #    lists a module's own depends-on lines (it does not recurse), so this
        #    naturally gives us tier-1 only -- the same depth spackLoadUnique's
        #    spack-based twin restricts itself to via the "spaces > 5" check.
        local exclude_pkgs="compiler-wrapper gcc gcc-runtime glibc"

        local root_base=$(echo "$selected_mod" | sed 's#/.*##')

        local dep_lines=$(module show "$selected_mod" 2>&1 | grep '^depends-on' | awk '{print $2}')

        local allowed_pkgs="$root_base"
        for dep in $dep_lines; do
            local dep_base=$(echo "$dep" | sed 's#/.*##')
            local skip=0
            for ex in $exclude_pkgs; do
                if [[ "$dep_base" == "$ex" ]]; then skip=1; break; fi
            done
            if [[ $skip -eq 0 ]]; then
                allowed_pkgs="$allowed_pkgs $dep_base"
            fi
        done

        # 6. Populate _DIR, _LIB_PATH, _VERSION, and _HASH -- but only for the
        #    allowed (root + tier-1, non-compiler) packages -- and rebuild
        #    LD_LIBRARY_PATH from the pre-load snapshot plus only those packages'
        #    lib dirs, instead of trusting whatever the cascading module load left
        #    in LD_LIBRARY_PATH.
        local py_script='
import os, json, sys

allowed = sys.argv[1].split()
root_base = sys.argv[2]

def mangle(pkg_base):
    name = pkg_base.upper().replace("-", "_")
    if name == "BINUTILS":
        name = "BINUTILS_E4S"
    return name

# Deps first, root last -- so once we prepend in this order, the root package
# lib dir ends up first (highest priority) in LD_LIBRARY_PATH, matching the
# priority ordering spackSetPackageRoot produces on the spack-based path.
ordered = [p for p in allowed if p != root_base]
if root_base in allowed:
    ordered.append(root_base)

lib_paths = []

for pkg_base in ordered:
    mangled = mangle(pkg_base)
    root_var = mangled + "_ROOT"
    v = os.environ.get(root_var)
    if not v or not os.path.isdir(v):
        continue

    print(f"export {mangled}_DIR=\"{v}\"")

    lib_path = None
    if os.path.isdir(os.path.join(v, "lib64")):
        lib_path = os.path.join(v, "lib64")
    elif os.path.isdir(os.path.join(v, "lib")):
        lib_path = os.path.join(v, "lib")

    if lib_path:
        print(f"export {mangled}_LIB_PATH=\"{lib_path}\"")
        lib_paths.append(lib_path)

    spec_file = os.path.join(v, ".spack", "spec.json")
    if os.path.isfile(spec_file):
        try:
            with open(spec_file) as f:
                d = json.load(f)

            nodes = d.get("spec", {}).get("nodes", [])
            if isinstance(d.get("spec"), list):
                nodes = d["spec"]

            bname = os.path.basename(v)
            root_node = next((n for n in nodes if bname.startswith(n.get("name", "") + "-")), nodes[0] if nodes else None)

            if root_node:
                ver = root_node.get("version", "")
                hsh = root_node.get("hash", "")
                if ver: print(f"export {mangled}_VERSION=\"{ver}\"")
                if hsh: print(f"export {mangled}_HASH=\"{hsh}\"")
        except Exception:
            pass

pre_ld = os.environ.get("__PRE_LOAD_LD_LIBRARY_PATH", "")
new_ld = ":".join(lib_paths + ([pre_ld] if pre_ld else []))
print(f"export LD_LIBRARY_PATH=\"{new_ld}\"")
'
        export __PRE_LOAD_LD_LIBRARY_PATH="$PRE_LOAD_LD_LIBRARY_PATH"
        eval "$(python3 -c "$py_script" "$allowed_pkgs" "$root_base")"
        unset __PRE_LOAD_LD_LIBRARY_PATH
    }

    spackLoadUniqueNoX() {
        spackLoadUnique "$@"
    }

    spackLoadUniqueNoR() {
        spackLoadUnique "$@"
    }

else

#This is necessary in every sub-script that loads spack packages.
command -v spack >/dev/null 2>&1 || { source /spack/share/spack/setup-env.sh; }
command -v spack >/dev/null 2>&1 || { echo "Failure: Spack not found. Exiting"; exit 1; }
source ${SPACK_ROOT}/share/spack/setup-env.sh
#source ./settings.sh
#echo $BASH_SOURCE
#echo dirname $BASH_SOURCE



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

	if [[ $E4S_TEST_SKIP_INTERNAL == "True" ]]; then
		if grep -q "spackTestRun" ./run.sh ; then
		    echo "Module setup error: Cannot run internal spack test for: $*" >&2
            export SPACK_LOAD_RESULT=216
            return 216
		fi
	fi

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

fi