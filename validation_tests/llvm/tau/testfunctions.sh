RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'

BRED='\033[1;31m'
BGREEN='\033[1;32m'
BBLUE='\033[1;34m'

NC='\033[0m'

output::err() {
echo "[$(date +'%Y-%m-%dT%H:%M:%S%z')]: $*" >&2
}

output::status() {
if [ "$2" -ne 0 ] ; then
    echo -ne "$BRED"
else
    echo -ne "$BGREEN"
fi

echo -e "$1: $2 $NC"
}

symbols::exists() {
return $([ -f "$SYMBOL_CACHE" ])
}

SYMBOL_CACHE=.symbols

symbols::analysis() {
    symbols::exists && return 0
    
    # Get the target LLVM from either the environment or the $PATH
    LLVM_INSTALL=$([ -n "$LLVM" ] && echo $LLVM || echo `which clang | awk -F"bin" {'print $1'}`)

    # Is the plugin installed somewhere else ?
    PLUGIN_PREFIX=$([ -n "$PLUGIN_DIR" ] && echo $PLUGIN_DIR || echo $LLVM_INSTALL/lib)

    if [ -z "$LLVM_INSTALL" -o -z "$PLUGIN_PREFIX" -o -z "$TAU_INSTALL" ] ; then
        output::err "Invalid parameters."
        output::err LLVM_INSTALL: \"$LLVM_INSTALL\"
        output::err PLUGIN_PREFIX: \"$PLUGIN_PREFIX\"
        output::err TAU_INSTALL: \"$TAU_INSTALL\"
        exit 1
    fi

    OUTPUT=`mktemp`
    ERRFILE=`mktemp`
    SOURCES=$1

    COMPILER=$LLVM_INSTALL/bin/clang++
    $COMPILER -o $OUTPUT \
        -O0 -g \
        $SOURCES \
        &> $ERRFILE

    SUCCESS=$?

    output::status "Compilation of sources for symbol analysis" $SUCCESS

    if [ $SUCCESS -ne 0 ] ; then
        cat $ERRFILE
        exit $SUCCESS
    fi

    nm -lC --defined-only $OUTPUT | grep \( | cut -d' ' -f3- | cut -d: -f1 | sed -e "s:$PWD:.:" > .symbols

    rm $OUTPUT $ERRFILE
}

symbols::file() {
    symbols::exists || output::err "Symbol database not found"; exit 1

    PROTOTYPE="$1"

    FILES=$(grep $SYMBOL_CACHE -e "$PROTOTYPE" | cut -f2)

    if [ $(echo "$FILES" | wc -l) -ne 1 ] ; then
        output::err "Prototype $PROTOTYPE matches multiple symbols"
    fi

    echo $FILES
}

symbols::match() {
    symbols::exists || output::err "Symbol database not found"; exit 1

    REGEX="$1"

    grep $SYMBOL_CACHE -e "^$(echo "$REGEX" | sed 's/.$//')" | cut -f1
}

# Set:
# - LLVM for the install of LLVM to use (default system accessible)
# - PLUGIN_DIR if the plugin is not installed with LLVM (default with the above LLVM)
# - TAU_INSTALL for the TAU install to use (no default)
compiletest() {
    # Get the target LLVM from either the environment or the $PATH
    LLVM_INSTALL=$([ -n "$LLVM" ] && echo $LLVM || echo `which clang | awk -F"bin" {'print $1'}`)

    # Is the plugin installed somewhere else ?
    PLUGIN_PREFIX=$([ -n "$PLUGIN_DIR" ] && echo $PLUGIN_DIR || echo $LLVM_INSTALL/lib)

    if [ -z "$LLVM_INSTALL" -o -z "$PLUGIN_PREFIX" -o -z "$TAU_INSTALL" ] ; then
        output::err "Invalid parameters."
        output::err LLVM_INSTALL: \"$LLVM_INSTALL\"
        output::err PLUGIN_PREFIX: \"$PLUGIN_PREFIX\"
        output::err TAU_INSTALL: \"$TAU_INSTALL\"
        exit 1
    fi

    FUNC_LIST=$1
    OUTPUT=$2
    SOURCES=$3

    OptionalC=${4:-C++}

    COMPILER=$LLVM_INSTALL/bin/clang++
    PLUGIN=$PLUGIN_PREFIX/TAU_Profiling_CXX.so

    if [ $OptionalC == "C" ]; then
        COMPILER=$LLVM_INSTALL/bin/clang
        PLUGIN=$PLUGIN_PREFIX/TAU_Profiling.so
    fi

    ERRFILE=`mktemp`

    $COMPILER -o $OUTPUT \
        -O3 -g \
        -fplugin=$PLUGIN \
        -mllvm \
        -tau-input-file=$FUNC_LIST \
        -L$TAU_INSTALL \
        -ldl -lTAU \
        -Wl,-rpath,$TAU_INSTALL \
        $SOURCES \
        &> $ERRFILE

    SUCCESS=$?

    output::status "Compilation of $OUTPUT" $SUCCESS

    if [ $SUCCESS -ne 0 ] ; then
        cat $ERRFILE
        exit $SUCCESS
    fi

    rm $ERRFILE
}

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

runtest(){
    export FUNC_LIST=$1
    export EXECUTABLE=$2

    export OUTFILE=`mktemp`

    runexec $EXECUTABLE && verifytest $FUNC_LIST

    unset OUTFILE

    unset EXECUTABLE
    unset FUNC_LIST
}

runexec (){
    ERRFILE=`mktemp`

    rm -f profile.*

    echo -e "${BBLUE}Basic instrumentation file - cpp${NC}"

    tau_exec "./$EXECUTABLE" 256 256 > $OUTFILE 2> $ERRFILE
    SUCCESS=$?

    output::status "Execution of $EXECUTABLE" $SUCCESS

    if [ $SUCCESS -ne 0 ] ; then
        cat $ERRFILE
    fi

    return $SUCCESS
}

verifytest () {
    inputfile=$1

    fExcluded=`sed '/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/{/BEGIN_EXCLUDE_LIST/{h;d};H;/END_EXCLUDE_LIST/{x;/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/p}};d' $inputfile |  sed -e 's/BEGIN_EXCLUDE_LIST//' -e 's/END_EXCLUDE_LIST//' -e '/^$/d'`
    fIncluded=`sed '/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/{/BEGIN_INCLUDE_LIST/{h;d};H;/END_INCLUDE_LIST/{x;/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/p}};d' $inputfile |  sed -e 's/BEGIN_INCLUDE_LIST//' -e 's/END_INCLUDE_LIST//'  -e '/^$/d'`
    fExcludedFile=`sed '/BEGIN_FILE_EXCLUDE_LIST/,/END_FILE_EXCLUDE_LIST/{/BEGIN_FILE_EXCLUDE_LIST/{h;d};H;/END_FILE_EXCLUDE_LIST/{x;/BEGIN_FILE_EXCLUDE_LIST/,/END_FILE_EXCLUDE_LIST/p}};d' $inputfile |  sed -e 's/BEGIN_FILE_EXCLUDE_LIST//' -e 's/END_FILE_EXCLUDE_LIST//' -e '/^$/d'`
    fIncludedFile=`sed '/BEGIN_FILE_INCLUDE_LIST/,/END_FILE_INCLUDE_LIST/{/BEGIN_FILE_INCLUDE_LIST/{h;d};H;/END_FILE_INCLUDE_LIST/{x;/BEGIN_FILE_INCLUDE_LIST/,/END_FILE_INCLUDE_LIST/p}};d' $inputfile |  sed -e 's/BEGIN_FILE_INCLUDE_LIST//' -e 's/END_FILE_INCLUDE_LIST//'  -e '/^$/d'`

    fInstrumented=`pprof -l | grep -v "Reading" | grep -v ".TAU application"`

    # There might be spaces in the function names: change the separator
    IFS=$'\n'

    incorrectInstrumentation=0
    for funcinclu in $fIncluded; do
        #echo "Checking instrumentation of $line"
        varinstrumented=1
        varexcluded=1
        varfileincluded=0
        varfileexcluded=1

        # First: check that we actually wanted to instrument the instrumented functions
        # in this example we have no wildcards so the matching is straightforward

        # echo "Instrumented function" $funcinclu

        echo $fInstrumented | grep -qFw "$funcinclu"
        varinstrumented=$?
        echo $fExcluded | grep -qFw "$funcinclu"
        varexcluded=$?
        # Where is it defined? Look into all the source files of this directory (might pass as parameters of the function later if necessary)
        # Matching is not as straightforward as with C.
        #	for file in $(ls  *.{cpp,h}); do
        for file in $(ls  *.{cpp,h,c}); do
            for line in $(cat $file); do
                matchname $funcinclu $line
                if [[ $matched == 0 ]] ; then
                    # echo "I have found " $funcinclu "in file" $file
                    definition=$file
                    break
                fi
            done
        done
        echo $definition

        if [ $(echo $fIncludedFile | wc -w) -gt 0 ]; then
            echo $fIncludedFile | grep -qFw "$definition"
            varfileincluded=$?
        fi
        echo $fExcludedFile | grep -qFw "$definition"
        varfileexcluded=$?

        # echo "Function " $funcinclu " is defined in file " $definition
        #if [[ $fIncludedFile =~ $definition ]] ; then varfileincluded=0 ; fi # good
        #if [[ $fExcludedFile =~ $definition ]] ; then varfileexcluded=1; fi  # bad

        echo $funcinclu
        echo $varinstrumented
        echo $varexcluded
        echo $varfileincluded
        echo $varfileexcluded

        if [ $varinstrumented -eq 0 ] && [ ! $varexcluded -eq 0 ] && [ $varfileincluded -eq 0 ] && [ ! $varfileexcluded -eq 0 ];
        then
            echo null > /dev/null
            echo -e "${BGREEN}Lawfully instrumented${NC}"
        elif [ $varinstrumented -eq 1 ] && [ ! $varexcluded -eq 0 ] && [ $varfileincluded -eq 0 ] && [ ! $varfileexcluded -eq 0 ];
        then
            ((incorrectInstrumentation=incorrectInstrumentation+1))
            echo -e "${BRED}Wrongfully not instrumented: included and not excluded${NC}"
        elif [ $varinstrumented -eq 0 ] && [ ! $varexcluded -eq 1 ];
        then
            ((incorrectInstrumentation=incorrectInstrumentation+1))
            echo -e "${BRED}Wrongfully instrumented: excluded${NC}"
        elif [ $varinstrumented -eq 0 ] && [ $varfileincluded -eq 1 ];
        then
            ((incorrectInstrumentation=incorrectInstrumentation+1))
            echo -e "${BRED}Wrongfully instrumented: source file is not included${NC}"
        elif [ $varinstrumented -eq 0 ] && [ ! $varfileexcluded -eq 1 ];
        then
            ((incorrectInstrumentation=incorrectInstrumentation+1))
            echo -e "${BRED}Wrongfully instrumented: source file is excluded${NC}"
        elif [ $varinstrumented -eq 1 ] && ([ ! $varexcluded -eq 1 ] || [ $varfileincluded -eq 1 ] || [ ! $varfileexcluded -eq 1 ]);
        then
            echo null > /dev/null
            echo -e "${BGREEN}Lawfully not instrumented: excluded or not included${NC}"
        else
            echo Uncovered case to implement
            ((incorrectInstrumentation=incorrectInstrumentation+1))
        fi

    done

    for funcinstru in $fInstrumented; do
        echo "Checking inclusion of $funcinstru"
        varincluded=1
        if echo $funcinstru | grep -qF "TAU";
        then
            continue
        fi

        echo $fIncluded | grep -qF "$funcinstru"; # We check if the function was indeed included
        varincluded=$?

        if [ $varincluded -gt 0 ];
        then
            ((incorrectInstrumentation=incorrectInstrumentation+1))
            echo -e "${BRED}Wrongfully instrumented: not included${NC}"
        fi
    done

    echo -n "Instrumentation of C++ code"
    if [ $incorrectInstrumentation -eq 0 ]; then
        echo -e "                       ${BGREEN}[PASSED]${NC}"
    else
        echo -e "                       ${BRED}[FAILED]${NC}"
    fi

    rm -f $OUTFILE
}

cevtest() {
    InputFile=$1
    Executable=$2
    SourceList=$3
    OptionalC=${4:-C++}

    if [ $# -lt 1 ]; then
        echo "Missing input file: stopping the test"
        exit
    fi

    if [ $(cat $1 | wc -l) -eq 0 ]; then
        echo -e "Input file doesn't exist: stopping the test"
        exit
    else
        echo -e "Input file detected"
    fi

    compiletest "$InputFile" "$Executable" "$SourceList" "$OptionalC"
    runtest "$InputFile" "$Executable"
}
