#!/bin/bash

. ./setup.sh

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'

BRED='\033[1;31m'
BGREEN='\033[1;32m'
BBLUE='\033[1;34m'

NC='\033[0m'

OUTFILE="toto"

rm profile.*
echo -e "${BBLUE}Basic instrumentation file - cpp${NC}"
tau_exec  -T serial,clang ./householder 256 256 &> $OUTFILE
RC=$?
echo -n "Execution of C++ instrumented code"
if [ $RC != 0 ]; then
    echo -e "                ${BRED}[FAILED]${NC}"
else
    echo -e "                ${BGREEN}[PASSED]${NC}"
fi


sed '/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/{/BEGIN_EXCLUDE_LIST/{h;d};H;/END_EXCLUDE_LIST/{x;/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/p}};d' ./functions_CXX_hh.txt |  sed -e 's/BEGIN_EXCLUDE_LIST//' -e 's/END_EXCLUDE_LIST//' -e '/^$/d' > Excluded
sed '/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/{/BEGIN_INCLUDE_LIST/{h;d};H;/END_INCLUDE_LIST/{x;/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/p}};d' ./functions_CXX_hh.txt |  sed -e 's/BEGIN_INCLUDE_LIST//' -e 's/END_INCLUDE_LIST//'  -e '/^$/d' > Included

fIncluded=./Included
fExcluded=./Excluded

pprof -l | grep -v "Reading" > Instrumented

fInstrumented=./Instrumented
incorrectInstrumentation=0



while read -r line ; do
    #echo "Checking intrumentation of $line"
    if grep -qF  "$line" $fInstrumented;
    then
	echo 0 > /dev/null
        #echo -e "${BGREEN}Instrumented${NC}"
    else
        if grep -qF "$line" $fExcluded; then
	    echo 0 > /dev/null
            #echo -e "${BRED}Excluded${NC}"
        else
            if echo $line | grep -q "#" ;then
                echo "'#' symbol, cannot test wildcard"
            else
                #echo -e "${BRED}Function wrongfully not instrumented${NC}"
                ((incorrectInstrumentation=incorrectInstrumentation+1))
            fi
        fi
    fi
done < $fIncluded

while read -r line ; do
    #echo "Checking exclusion of $line"
    if grep -qF  "$line" $fInstrumented;
    then
        #echo -e "${BRED}Function wrongfully instrumented${NC}"
        ((incorrectInstrumentation=incorrectInstrumentation+1))
    else
	echo 0 > /dev/null
        #echo -e "${BGREEN}Excluded${NC}"
    fi
done < $fExcluded

echo -n "Profiled the functions"
if [ $incorrectInstrumentation -eq 0 ]; then
    echo -e "		                  ${BGREEN}[PASSED]${NC}"

else
    echo -e "		                  ${BGREEN}[PASSED]${NC}"
fi


rm $OUTFILE

rm profile.*
echo -e "${BBLUE}Basic instrumentation file - c${NC}"
tau_exec  -T serial,clang ./mm_c &> $OUTFILE
RC=$?
echo -n "Execution of C instrumented code"
if [ $RC != 0 ]; then
    echo -e "                  ${BRED}[FAILED]${NC}"
else
    echo -e "                  ${BGREEN}[PASSED]${NC}"
fi

sed '/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/{/BEGIN_EXCLUDE_LIST/{h;d};H;/END_EXCLUDE_LIST/{x;/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/p}};d' ./functions_C_mm.txt |  sed -e 's/BEGIN_EXCLUDE_LIST//' -e 's/END_EXCLUDE_LIST//' -e '/^$/d' > Excluded
sed '/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/{/BEGIN_INCLUDE_LIST/{h;d};H;/END_INCLUDE_LIST/{x;/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/p}};d' ./functions_C_mm.txt |  sed -e 's/BEGIN_INCLUDE_LIST//' -e 's/END_INCLUDE_LIST//'  -e '/^$/d' > Included

fIncluded=./Included
fExcluded=./Excluded

pprof -l | grep -v "Reading" > Instrumented

fInstrumented=./Instrumented
incorrectInstrumentation=0



while read -r line ; do
    #echo "Checking intrumentation of $line"
    if grep -qF  "$line" $fInstrumented;
    then
	echo 0 > /dev/null
        #echo -e "${BGREEN}Instrumented${NC}"
    else
        if grep -qF "$line" $fExcluded; then
	    echo 0 > /dev/null
            #echo -e "${BRED}Excluded${NC}"
        else
            if echo $line | grep -q "#" ;then
                echo "'#' symbol, cannot test wildcard"
            else
                #echo -e "${BRED}Function wrongfully not instrumented${NC}"
                ((incorrectInstrumentation=incorrectInstrumentation+1))
            fi
        fi
    fi
done < $fIncluded

while read -r line ; do
    #echo "Checking exclusion of $line"
    if grep -qF  "$line" $fInstrumented;
    then
        #echo -e "${BRED}Function wrongfully instrumented${NC}"
        ((incorrectInstrumentation=incorrectInstrumentation+1))
    else
	echo 0 > /dev/null
        #echo -e "${BGREEN}Excluded${NC}"
    fi
done < $fExcluded

echo -n "Profiled the functions"
if [ $incorrectInstrumentation -eq 0 ]; then
    echo -e "		                  ${BGREEN}[PASSED]${NC}"

else
    echo -e "		                  ${BRED}[FAILED]${NC}"
fi


# This test doesn't support the use of wildcards, so these testcases are irrelevant
#rm $OUTFILE
#
#
#rm profile.*
#echo -e "${BBLUE}Regular expressions instrumentation file${NC}"
#tau_exec  -T serial,clang ./householder-regex &> $OUTFILE
#RC=$?
#echo -n "Execution of C++ instrumented code"
#if [ $RC != 0 ]; then
#    echo -e "                ${BRED}[FAILED]${NC}"
#else
#    echo -e "                ${BGREEN}[PASSED]${NC}"
#fi
#
#
#sed '/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/{/BEGIN_EXCLUDE_LIST/{h;d};H;/END_EXCLUDE_LIST/{x;/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/p}};d' ./functions_CXX_hh_regex.txt |  sed -e 's/BEGIN_EXCLUDE_LIST//' -e 's/END_EXCLUDE_LIST//' -e '/^$/d' > Excluded
#sed '/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/{/BEGIN_INCLUDE_LIST/{h;d};H;/END_INCLUDE_LIST/{x;/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/p}};d' ./functions_CXX_hh_regex.txt |  sed -e 's/BEGIN_INCLUDE_LIST//' -e 's/END_INCLUDE_LIST//'  -e '/^$/d' > Included
#
#fIncluded=./Included
#fExcluded=./Excluded
#
#pprof -l | grep -v "Reading" > Instrumented
#
#fInstrumented=./Instrumented
#incorrectInstrumentation=0
#
#
#
#while read -r line ; do
#    #echo "Checking intrumentation of $line"
#    if grep -qF  "$line" $fInstrumented;
#    then
#	echo 0 > /dev/null
#        #echo -e "${BGREEN}Instrumented${NC}"
#    else
#        if grep -qF "$line" $fExcluded; then
#	    echo 0 > /dev/null
#            #echo -e "${BRED}Excluded${NC}"
#        else
#            if echo $line | grep -q "#" ;then
#	        echo 0 > /dev/null
#                #echo "'#' symbol, cannot test wildcard"
#            else
#                #echo -e "${BRED}Function wrongfully not instrumented${NC}"
#                ((incorrectInstrumentation=incorrectInstrumentation+1))
#            fi
#        fi
#    fi
#done < $fIncluded
#
#while read -r line ; do
#    #echo "Checking exclusion of $line"
#    if grep -qF  "$line" $fInstrumented;
#    then
#        #echo -e "${BRED}Function wrongfully instrumented${NC}"
#        ((incorrectInstrumentation=incorrectInstrumentation+1))
#    else
#	echo 0 > /dev/null
#        #echo -e "${BGREEN}Excluded${NC}"
#    fi
#done < $fExcluded
#
#echo -n "Profiled the functions"
#if [ $incorrectInstrumentation -eq 0 ]; then
#    echo -e "		                  ${BGREEN}[PASSED]${NC}"
#
#else
#    echo -e "		                  ${BRED}[FAILED]${NC}"
#fi
#
#
#rm profile.*
#
#
#tau_exec  -T serial,clang ./mm_c_regex &> $OUTFILE
#RC=$?
#echo -n "Execution of C instrumented code"
#if [ $RC != 0 ]; then
#    echo -e "                  ${BRED}[FAILED]${NC}"
#else
#    echo -e "                  ${BGREEN}[PASSED]${NC}"
#fi
#
#sed '/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/{/BEGIN_EXCLUDE_LIST/{h;d};H;/END_EXCLUDE_LIST/{x;/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/p}};d' ./functions_C_mm_regex.txt |  sed -e 's/BEGIN_EXCLUDE_LIST//' -e 's/END_EXCLUDE_LIST//' -e '/^$/d' > Excluded
#sed '/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/{/BEGIN_INCLUDE_LIST/{h;d};H;/END_INCLUDE_LIST/{x;/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/p}};d' ./functions_C_mm_regex.txt |  sed -e 's/BEGIN_INCLUDE_LIST//' -e 's/END_INCLUDE_LIST//'  -e '/^$/d' > Included
#
#fIncluded=./Included
#fExcluded=./Excluded
#
#pprof -l | grep -v "Reading" > Instrumented
#
#fInstrumented=./Instrumented
#incorrectInstrumentation=0
#
#
#
#while read -r line ; do
#    #echo "Checking intrumentation of $line"
#    if grep -qF  "$line" $fInstrumented;
#    then
#	echo 0 > /dev/null
#        #echo -e "${BGREEN}Instrumented${NC}"
#    else
#        if grep -qF "$line" $fExcluded; then
#	    echo 0 > /dev/null
#            #echo -e "${BRED}Excluded${NC}"
#        else
#            if echo $line | grep -q "#" ;then
#	        echo 0 > /dev/null
#                #echo "'#' symbol, cannot test wildcard"
#            else
#                #echo -e "${BRED}Function wrongfully not instrumented${NC}"
#                ((incorrectInstrumentation=incorrectInstrumentation+1))
#            fi
#        fi
#    fi
#done < $fIncluded
#
#while read -r line ; do
#    #echo "Checking exclusion of $line"
#    if grep -qF  "$line" $fInstrumented;
#    then
#        #echo -e "${BRED}Function wrongfully instrumented${NC}"
#        ((incorrectInstrumentation=incorrectInstrumentation+1))
#    else
#	echo 0 > /dev/null
#        #echo -e "${BGREEN}Excluded${NC}"
#    fi
#done < $fExcluded
#
#echo -n "Profiled the functions"
#if [ $incorrectInstrumentation -eq 0 ]; then
#    echo -e "		                  ${BGREEN}[PASSED]${NC}"
#
#else
#    echo -e "		                  ${BRED}[FAILED]${NC}"
#fi

rm $OUTFILE
rm profile.*
rm $fIncluded
rm $fExcluded
rm $fInstrumented

