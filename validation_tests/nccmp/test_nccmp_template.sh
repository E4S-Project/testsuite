#!/bin/bash

echo $I. $HELP

$srcdir/test_nccmp_setup.sh $DATA $I
TRUTH=$srcdir/stderr$I.txt
NCGEN_VER=$(ncgen -v 2>&1 | grep -e version | cut -d' ' -f4)

if [[ ${NCGEN_VER:0:5} < "4.3.3" ]]; then
    LEGACY_TRUTH=$srcdir/stderr$I.pre433.txt
    if [ -e $LEGACY_TRUTH ]; then
        TRUTH=$LEGACY_TRUTH
    fi
fi

LOG=stderr$I.tmp
DIFF="diff $TRUTH $LOG"
RUN="$($srcdir/nccmp.sh) $ARGS > $LOG 2>&1"
eval $RUN

if test "$?" = "$EXPECT"; then :; else
    echo
    echo "Expected exit code $EXPECT."
    echo "Test that failed: "
    echo "$RUN"
    exit 1
fi

if [ -n $SORT ]; then
    LOG2=stderr${I}.2.tmp
    LC_ALL=C sort $SORT $LOG > $LOG2
    mv $LOG2 $LOG
fi

if [ -e "$TRUTH" ]; then
    eval $DIFF

    if test "$?" = "0"; then :; else
        echo "Test that failed: "
        echo "$RUN"
        echo "$DIFF"
        exit 1
    fi
else
    echo "ERROR: $TRUTH does not exist."
    exit 1
fi
