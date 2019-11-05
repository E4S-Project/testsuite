#This is necessary in every sub-script that loads spack packages.
command -v spack >/dev/null 2>&1 || { source /spack/share/spack/setup-env.sh; }
command -v spack >/dev/null 2>&1 || { echo "Failure: Spack not found. Exiting"; exit 1; }

oneSpackHash(){ echo "/`spack find -l $1 | tail -n1 | awk '{print $1;}'`" ;  }
spackLoadUnique(){ spack load -r `oneSpackHash $1` ; }

