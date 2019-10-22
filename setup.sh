#This is necessary in every sub-script that loads spack packages.
source /spack/share/spack/setup-env.sh
oneSpackHash(){ echo "/`spack find -l $1 | tail -n1 | awk '{print $1;}'`" ;  }
