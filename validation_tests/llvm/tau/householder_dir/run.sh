#!/bin/bash

# For commented code, read cev.sh

. ./setup.sh


RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'

BRED='\033[1;31m'
BGREEN='\033[1;32m'
BBLUE='\033[1;34m'

NC='\033[0m'

OUTFILE="toto"

# Match a line coming from the input file with a line coming from the source code
# returns 0 in the variable $matched if the lines match, 1 otherwise
# Does not consider wildcards
# $1: function prototype
# $2: line
function matchname(){
    funcproto=$1
    line=$2

    # Does it end like a function definition?
    # oddly enough, grep is easier for regex matching here
    f1=`echo $line | grep -E "*\)[[:space:]]*\{[[:space:]]*$" | wc -l`
    f2=`echo $line | grep -E "*\)[[:space:]]*$" | wc -l`
    if [[ $(($f1+$f2)) == 0 ]] ; then
	matched=1
	return
    fi

    # Does it start like a function definition?
    nf=`echo $line | cut -d "(" -f 1 | awk {'print NF'}`
    if [[ ! $nf == 2 ]] ; then
	matched=1
	return
    fi

    # Does the input file provide the returned type (optional)
    # if we have 2 fields before the '(' -> yes, otherwise no
    nf=`echo $funcproto | cut -d "(" -f 1 | awk {'print NF'}`

    if [[ $nf == 2 ]] ; then
	# TODO test this
	# does it return the requested type?
	type1=`echo $line | cut -f 1 -d " "`
	type2=`echo $funcproto | cut -f 1 -d " "`
	if [[ ! $type1 == $type2 ]] ; then
	    matched=1
	    return
	fi
    fi

    # Is this the same function name?
    name1=`echo $line | cut -d '(' -f 1 | awk -F " " {'print $NF'}`
    name2=`echo $funcproto | cut -d '(' -f 1 | awk -F " " {'print $NF'}`
    if [[ ! $name1 == $name2 ]] ; then
	matched=1
	return
    fi

    # Which types do we have between the parenthesis

    linetypes=`echo $line |cut -d "(" -f 2 | cut -d ")" -f 1 | awk -F " " {'for(i = 1 ; i < NF ; i+= 2 ) { printf $i " "} '}`
    prototypes=`echo $funcproto | cut -d "(" -f 2 | cut -d ")" -f 1  | sed 's/,//g'`

    # Same number of arguments?
    n1=`echo $linetypes | awk {'print NF'}`
    n2=`echo $prototypes | awk {'print NF'}`
    if [[ ! $n1 == $n2 ]] ; then
	matched=1
	return
    fi

    # We need this to make lists we can manipulate easily
    OLDIFS=$IFS
    IFS=' '
    lt=($linetypes)
    pt=($prototypes)
    IFS=$OLDIFS

    # Compare these lists term by term
    for i in "${!lt[@]}"; do
	if [[ ! ${pt[i]} == ${lt[i]} ]] ; then
	    matched=1
	    return
	fi
    done

    matched=0
}

runtest () {
    inputfile=$1
    executable=$2

    rm profile.*
    echo -e "${BBLUE}Basic instrumentation file - cpp${NC}"
    tau_exec  -T serial,clang ./$executable 256 256 &> $OUTFILE
    RC=$?
    echo -n "Execution of C++ instrumented code"
    if [ $RC != 0 ]; then
        echo -e "                ${BRED}[FAILED]${NC}"
    else
        echo -e "                ${BGREEN}[PASSED]${NC}"
    fi

    fExcluded=`sed '/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/{/BEGIN_EXCLUDE_LIST/{h;d};H;/END_EXCLUDE_LIST/{x;/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/p}};d' $inputfile |  sed -e 's/BEGIN_EXCLUDE_LIST//' -e 's/END_EXCLUDE_LIST//' -e '/^$/d'`
    fIncluded=`sed '/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/{/BEGIN_INCLUDE_LIST/{h;d};H;/END_INCLUDE_LIST/{x;/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/p}};d' $inputfile |  sed -e 's/BEGIN_INCLUDE_LIST//' -e 's/END_INCLUDE_LIST//'  -e '/^$/d'`
    fExcludedFile=`sed '/BEGIN_FILE_EXCLUDE_LIST/,/END_FILE_EXCLUDE_LIST/{/BEGIN_FILE_EXCLUDE_LIST/{h;d};H;/END_FILE_EXCLUDE_LIST/{x;/BEGIN_FILE_EXCLUDE_LIST/,/END_FILE_EXCLUDE_LIST/p}};d' $inputfile |  sed -e 's/BEGIN_FILE_EXCLUDE_LIST//' -e 's/END_FILE_EXCLUDE_LIST//' -e '/^$/d'`
    fIncludedFile=`sed '/BEGIN_FILE_INCLUDE_LIST/,/END_FILE_INCLUDE_LIST/{/BEGIN_FILE_INCLUDE_LIST/{h;d};H;/END_FILE_INCLUDE_LIST/{x;/BEGIN_FILE_INCLUDE_LIST/,/END_FILE_INCLUDE_LIST/p}};d' $inputfile |  sed -e 's/BEGIN_FILE_INCLUDE_LIST//' -e 's/END_FILE_INCLUDE_LIST//'  -e '/^$/d'`


    #
    #sed '/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/{/BEGIN_EXCLUDE_LIST/{h;d};H;/END_EXCLUDE_LIST/{x;/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/p}};d' ./functions_CXX_hh_files.txt |  sed -e 's/BEGIN_EXCLUDE_LIST//' -e 's/END_EXCLUDE_LIST//' -e '/^$/d' > $fExcluded
    #sed '/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/{/BEGIN_INCLUDE_LIST/{h;d};H;/END_INCLUDE_LIST/{x;/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/p}};d' ./functions_CXX_hh_files.txt |  sed -e 's/BEGIN_INCLUDE_LIST//' -e 's/END_INCLUDE_LIST//'  -e '/^$/d' > $fIncluded
    #sed '/BEGIN_FILE_EXCLUDE_LIST/,/END_FILE_EXCLUDE_LIST/{/BEGIN_FILE_EXCLUDE_LIST/{h;d};H;/END_FILE_EXCLUDE_LIST/{x;/BEGIN_FILE_EXCLUDE_LIST/,/END_FILE_EXCLUDE_LIST/p}};d' ./functions_CXX_hh_files.txt |  sed -e 's/BEGIN_FILE_EXCLUDE_LIST//' -e 's/END_FILE_EXCLUDE_LIST//' -e '/^$/d' > $fExcludedFile
    #sed '/BEGIN_FILE_INCLUDE_LIST/,/END_FILE_INCLUDE_LIST/{/BEGIN_FILE_INCLUDE_LIST/{h;d};H;/END_FILE_INCLUDE_LIST/{x;/BEGIN_FILE_INCLUDE_LIST/,/END_FILE_INCLUDE_LIST/p}};d' ./functions_CXX_hh_files.txt |  sed -e 's/BEGIN_FILE_INCLUDE_LIST//' -e 's/END_FILE_INCLUDE_LIST//'  -e '/^$/d' > $fIncludedFile


    fInstrumented=`pprof -l | grep -v "Reading" | grep -v ".TAU application"`

    # There might be spaces in the function names: change the separator
    IFS=$'\n' 
	
    incorrectInstrumentation=0
    for funcinstru in $fInstrumented; do
        #echo "Checking instrumentation of $line"
        varinstrumented=1
        varexcluded=0
        varfileincluded=0
        varfileexcluded=1

	# First: check that we actually wanted to instrument the instrumented functions
	# in this example we have no wildcards so the matching is straightforward
	
	# echo "Instrumented function" $funcinstru

	if [[ $fIncluded =~ $funcinstru ]] ; then varinstrumented=0 ; fi # good
	if [[ $fExcluded =~ $funcinstru ]] ; then varexcluded=1; fi      # bad

	# Where is it defined? Look into all the source files of this directory (might pass as parameters of the function later if necessary)
	# Matching is not as straightforward as with C.
	#	for file in $(ls  *.{cpp,h}); do
	for file in $(ls  *.cpp); do
	    for line in $(cat $file); do
		matchname $funcinstru $line
		if [[ $matched == 0 ]] ; then
		    # echo "I have found " $funcinstru "in file" $file
		    definition=$file
		    break
		fi
	    done
	done

	# echo "Function " $funcinstru " is defined in file " $definition

	if [[ $fIncludedFile =~ $definition ]] ; then varfileincluded=0 ; fi # good
	if [[ $fExcludedFile =~ $definition ]] ; then varfileexcluded=1; fi  # bad


        if [ $varinstrumented -eq 0 ] && [ ! $varexcluded -eq 0 ] && [ $varfileincluded -eq 0 ] && [ ! $varfileexcluded -eq 0 ];
        then
            echo null > /dev/null
            # echo -e "${BGREEN}Lawfully instrumented${NC}"
        elif [ $varinstrumented -eq 1 ] && [ ! $varexcluded -eq 0 ] && [ $varfileincluded -eq 0 ] && [ ! $varfileexcluded -eq 0 ];
        then
            ((incorrectInstrumentation=incorrectInstrumentation+1))
            # echo -e "${BRED}Wrongfully not instrumented: included and not excluded${NC}"
        elif [ $varinstrumented -eq 0 ] && [ ! $varexcluded -eq 1 ];
        then
            ((incorrectInstrumentation=incorrectInstrumentation+1))
            # echo -e "${BRED}Wrongfully instrumented: excluded${NC}"
        elif [ $varinstrumented -eq 0 ] && [ $varfileincluded -eq 1 ];
        then
            ((incorrectInstrumentation=incorrectInstrumentation+1))
            # echo -e "${BRED}Wrongfully instrumented: source file is not included${NC}"
        elif [ $varinstrumented -eq 0 ] && [ ! $varfileexcluded -eq 1 ];
        then
            ((incorrectInstrumentation=incorrectInstrumentation+1))
            # echo -e "${BRED}Wrongfully instrumented: source file is excluded${NC}"
        elif [ $varinstrumented -eq 1 ] && ([ ! $varexcluded -eq 1 ] || [ $varfileincluded -eq 1 ] || [ ! $varfileexcluded -eq 1 ]);
        then
            echo null > /dev/null
            # echo -e "${BGREEN}Lawfully not instrumented: excluded or not included${NC}"
        else
            echo Uncovered case to implement 
            ((incorrectInstrumentation=incorrectInstrumentation+1))
        fi

    done

    for funcinstru in $fInstrumented; do
        #echo "Checking inclusion of $line"
        varincluded=0
        if echo $line | grep -qF "TAU";
        then
            continue
        fi
	if [[ $fIncluded =~ $funcinstru ]] ; then varincluded=1; fi  
        if [ $varincluded -gt 0 ];
        then
            ((incorrectInstrumentation=incorrectInstrumentation+1))
            #echo -e "${BRED}Wrongfully instrumented, not included${NC}"
        fi
    done 

    return $incorrectInstrumentation
}

runtest "functions_C_files2.txt" "householder"
runoutput=$?

echo -n "Instrumentation of C++ code"
if [ $runoutput -eq 0 ]; then
    echo -e "                       ${BGREEN}[PASSED]${NC}"
else
    echo -e "                       ${BRED}[FAILED]${NC}"
fi

runtest "functions_C_files3.txt" "householderfileWC"
runoutput=$?

echo -n "Instrumentation of C++ code"
if [ $runoutput -eq 0 ]; then
    echo -e "                       ${BGREEN}[PASSED]${NC}"
else
    echo -e "                       ${BRED}[FAILED]${NC}"
fi
runtest "functions_CXX_hh_files.txt" "householdercxx"
runoutput=$?

echo -n "Instrumentation of C++ code"
if [ $runoutput -eq 0 ]; then
    echo -e "                       ${BGREEN}[PASSED]${NC}"
else
    echo -e "                       ${BRED}[FAILED]${NC}"
fi


rm $OUTFILE
#rm profile.*

