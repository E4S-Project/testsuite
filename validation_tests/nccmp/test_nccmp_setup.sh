#!/bin/bash 
#   Copyright (C) 2004-2015 remik @ fastmail . com
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2, or (at your option)
#   any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; see the file COPYING.
#   If not, write to the Free Software Foundation,
#   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  
###############################################################################
# Help
#
# Adding new CDL Files
# - Add an entry in format() FMT with appropriate format.

if [ $# -lt 2 ]
then
    echo "Usage : $0 [compound|encoding|group|missing|nan|NUMBER|pad|subgroups] <id>"
    echo
    echo "Examples:"
    echo "  $0 21 68  # Will generate files test21{a,b}.68.nc"
    echo "  $0 pad 10 # Will create files padded.10.nc and unpadded.10.nc"
    exit 1
fi
DATA=$1
ID=$2

###############################################################################
function format() {
    if [ $# -lt 2 ]; then
        echo "Usage : $0 <file-id> <unique-id>"
        echo "<file-id>: double digit string like 01."
        exit 1
    fi
    IDX=$1
    ID=$2
    let "index=10#${IDX}" # Ensure not octal when ID==08.
    
    NC_FORMATS=$($srcdir/ncgen_formats.sh $ID)
    NC3=$(echo $NC_FORMATS | cut -f1 -d' ')
    NC4=$(echo $NC_FORMATS | cut -f2 -d' ')
    FMT=(0    $NC3 $NC3 $NC3 $NC3
         $NC3 $NC3 $NC3 $NC3 $NC3
         $NC4 $NC4 $NC4 $NC4 $NC4
         $NC4 $NC4 $NC4 $NC4 $NC4
         $NC4 $NC4 $NC4 $NC4 $NC3)
    
    echo ${FMT[${index}]}
}

###############################################################################
function ncgen_version() {
    VER=$(ncgen -v 2>&1 | grep -e version | cut -d' ' -f4)
    echo $VER
}

###############################################################################
NCGEN=`which ncgen 2> /dev/null`
if test "$?" = "0"; then :; else 
    echo "ERROR: ncgen command not found."
    exit 1
fi

case "$DATA" in
    compound)
        echo For compound type with atomic arrays.
        ./make_compound_array_atomic 0 make_compound_array_atomic1.$ID.nc
        ./make_compound_array_atomic 1 make_compound_array_atomic2.$ID.nc
        echo For compound type with user-type arrays.
        ./make_compound_array_user_type 1 make_compound_array_user_type1.$ID.nc 
        ./make_compound_array_user_type 2 make_compound_array_user_type2.$ID.nc
        echo For compound type nested with atomics.
        ./make_compound_nest_atomic 0 make_compound_nest_atomic1.$ID.nc
        ./make_compound_nest_atomic 1 make_compound_nest_atomic2.$ID.nc
        echo For compound/vlen nested types. ncgen cannot create these.
        ./make_compound_vlen_nest 0 make_compound_vlen_nest1.$ID.nc
        ./make_compound_vlen_nest 100 make_compound_vlen_nest2.$ID.nc
        ;;
    encoding)
        echo To compare encoding features in hdf format.
        ./make_nc4_encodings 0 not_encoded.$ID.nc
        ./make_nc4_encodings 1 encoded.$ID.nc
        ;;
    group)
        echo For group tests, require nc4.
        fmt=$(format 10 $ID)
        $NCGEN -k $fmt -o group1.$ID.nc -x $srcdir/group1.cdl
        $NCGEN -k $fmt -o group2.$ID.nc -x $srcdir/group2.cdl
        ;;
    missing)
        echo Create two files with different missing_values.
        $NCGEN -o missing_value1.$ID.nc $srcdir/missing_value1.cdl 
        $NCGEN -o missing_value2.$ID.nc $srcdir/missing_value2.cdl 
        $NCGEN -o _FillValue1.$ID.nc $srcdir/_FillValue1.cdl
        ;;
    nan) 
        echo Files for NaN features.
        ./make_nans test_nans1a.$ID.nc 0 # Without nans.
        ./make_nans test_nans1b.$ID.nc 1 # With nans.
        ./make_nans test_nans1c.$ID.nc 1 # With nans. Just separate file from version 1b.
        ;;
    pad) 
        echo Create the head pad test files for comparison of different lengths.
        ./padheader unpadded.$ID.nc 0 
        ./padheader padded.$ID.nc 2048
        ;;
    subgroups)
        echo For subgroup tests, require nc4.
        fmt=$(format 10 $ID)
        $NCGEN -k $fmt -o subgroups1a.$ID.nc -x $srcdir/subgroups1a.cdl
        $NCGEN -k $fmt -o subgroups1b.$ID.nc -x $srcdir/subgroups1b.cdl
        ;;
    *)
        echo Create basic netcdf test files with ncgen.
        for SUFFIX in a b; do
            # Check for backwards compatible test files. 
            PATCH=""
            NCGEN_VER=$(ncgen_version)
            if [[ ${NCGEN_VER:0:5} < "4.3.3" ]]; then
                if [ -e test${DATA}${SUFFIX}.pre433.cdl ]; then
                    PATCH=.pre433
                elif [ -e test${DATA}${SUFFIX}.pre413.cdl ]; then
                    PATCH=.pre413
                fi
            fi
            fin=$srcdir/test${DATA}${SUFFIX}${PATCH}.cdl
            fout=test${DATA}${SUFFIX}.$ID.nc
            fmt=$(format $DATA $ID)
            CMD="ncgen -k $fmt -o $fout $fin"
            echo $CMD
            eval $CMD
        done
        if [ $DATA = "01" ] ; then
            echo Create file in nc4 format.
            fmt=$(format 10 $ID)
            ncgen -k $fmt -o test${DATA}c.$ID.nc $srcdir/test${DATA}a.cdl 
        fi
        ;;
esac


 


