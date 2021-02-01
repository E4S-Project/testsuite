RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'

BRED='\033[1;31m'
BGREEN='\033[1;32m'
BBLUE='\033[1;34m'

NC='\033[0m'

# Set:
# - LLVM for the install of LLVM to use (default system accessible)
# - PLUGIN_DIR if the plugin is not installed with LLVM (default with the above LLVM)
# - TAU_INSTALL for the TAU install to use (no default)
environment::enter() {
    # Get the target LLVM from either the environment or the $PATH
    export TEST_LLVM_INSTALL=$([ -n "$LLVM" ] && echo $LLVM || echo `which clang | awk -F"bin" {'print $1'}`)

    # Is the plugin installed somewhere else ?
    export TEST_PLUGIN_PREFIX=$([ -n "$PLUGIN_DIR" ] && echo $PLUGIN_DIR || echo $LLVM_INSTALL/lib)

    export TEST_TAU_INSTALL=$TAU_INSTALL

    if [ -z "$TEST_LLVM_INSTALL" -o -z "$TEST_PLUGIN_PREFIX" -o -z "$TEST_TAU_INSTALL" ] ; then
        output::err "Invalid parameters."
        output::err Using LLVM: \"$TEST_LLVM_INSTALL\"
        output::err Using plugin in: \"$TEST_PLUGIN_PREFIX\"
        output::err Using libTAU.so from: \"$TEST_TAU_INSTALL\"
        output::err If any of those values are erroneous, please set the
        output::err LLVM, PLUGIN_DIR or TAU_INSTALL environment variables.
        exit 1
    fi
}

environment::exit() {
    unset TEST_LLVM_INSTALL TEST_PLUGIN_PREFIX TEST_TAU_INSTALL
}

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

# File in which the project's symbols are to be stored
SYMBOL_CACHE=.symbols

# Check for the existence of the symbol cache
symbols::exists() {
[ -f "$SYMBOL_CACHE" ]
}

# Generate the project's symbol database if not present
symbols::analysis() {
    symbols::exists && return 0

    environment::enter

    OUTPUT=`mktemp`
    ERRFILE=`mktemp`

    OptionalC=${2:-C++}
    
    COMPILER=$TEST_LLVM_INSTALL/bin/clang++
    
    if [ $OptionalC == "C" ]; then
        COMPILER=$TEST_LLVM_INSTALL/bin/clang
    fi    
    
    $COMPILER -o $OUTPUT \
        -O0 -g  -lm\
        $SOURCES \
        &> $ERRFILE

    SUCCESS=$?

    output::status "Compilation of sources for symbol analysis" $SUCCESS

    if [ $SUCCESS -ne 0 ] ; then
        cat "$ERRFILE"
        environment::exit
        exit $SUCCESS
    fi

    nm -lC --defined-only $OUTPUT   `# Get debug symbols with addresses` \
        | cut -d' ' -f3-                      `# Remove the hex address` \
        | cut -d: -f1                         `# Remove the line number` \
        | grep -e $'\t'                   `# Keep lines with a filename` \
        | sed -e "s:$PWD:.:"                 `# Make the paths relative` \
        > $SYMBOL_CACHE

    rm $OUTPUT $ERRFILE

    environment::exit
}

# Get the file in which a symbol is defined
# The first argument is the EXACT symbol to query
# $1: symbol
symbols::file() {
symbols::exists || { output::err Database file not found; exit 1; }

    PROTOTYPE="$1"

    FILES=$(grep $SYMBOL_CACHE -e "^$(echo $PROTOTYPE | sed -s 's:*:\\*:g')" | cut -f2)

    if [ $(echo "$FILES" | wc -l) -ne 1 ] ; then
        output::err "Prototype $PROTOTYPE matches multiple symbols"
    fi

    echo $FILES
}

# Expand a REGEX
# Outputs matches for a symbol REGEX (Using #)
# $1: prototype regex
symbols::match() {
symbols::exists || { output::err Database file not found; exit 1; }

    REGEX="$1"

    grep $SYMBOL_CACHE -e "^$(echo "$REGEX" | sed 's/.$//')" | cut -f1
}

compiletest() {
    environment::enter

    FUNC_LIST=$1
    OUTPUT=$2
    SOURCES=$3

    OptionalC=${4:-C++}

    COMPILER=$TEST_LLVM_INSTALL/bin/clang++
    PLUGIN=$TEST_PLUGIN_PREFIX/TAU_Profiling_CXX.so

    if [ $OptionalC == "C" ]; then
        COMPILER=$TEST_LLVM_INSTALL/bin/clang
        PLUGIN=$TEST_PLUGIN_PREFIX/TAU_Profiling.so
    fi

    ERRFILE=`mktemp`

    $COMPILER  -o $OUTPUT \
        -O3 -g \
        -fplugin=$PLUGIN \
        -mllvm \
        -tau-input-file=$FUNC_LIST \
        -L$TEST_TAU_INSTALL \
        -ldl -lTAU -lm \
        -Wl,-rpath,$TEST_TAU_INSTALL \
        $SOURCES \
        &> $ERRFILE
    SUCCESS=$?

    output::status "Compilation of $OUTPUT" $SUCCESS

    if [ $SUCCESS -ne 0 ] ; then
        cat "$ERRFILE"
        environment::exit
        exit $SUCCESS
    fi

    rm $ERRFILE
    environment::exit
}

runtest() {
    export FUNC_LIST=$1
    export EXECUTABLE=$2

    export OUTFILE=`mktemp`

    OptionalC=${3:-C++}
    runexec $EXECUTABLE && verifytest $FUNC_LIST $OptionalC

    unset OUTFILE

    unset EXECUTABLE
    unset FUNC_LIST
}

runexec() {
    ERRFILE=`mktemp`

    rm -f profile.*

    echo -e "${BBLUE}Basic instrumentation file ${NC}"

    tau_exec "./$EXECUTABLE" 256 256 > $OUTFILE 2> $ERRFILE
    SUCCESS=$?

    output::status "Execution of $EXECUTABLE" $SUCCESS

    if [ $SUCCESS -ne 0 ] ; then
        cat "$ERRFILE"
    fi

    return $SUCCESS
}

checkwildcard() {
    fIncluded=$1
    for funcinclu in $fIncluded; do
       if echo $funcinclu | grep -qF "#"; then
           functoadd="$(symbols::match $funcinclu)"
           echo "$functoadd"
       fi 
   done
   echo "$fIncluded"
}


verifytest() {
    inputfile=$1
    OptionalC=${2:-C++}

    fExcluded=`sed '/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/{/BEGIN_EXCLUDE_LIST/{h;d};H;/END_EXCLUDE_LIST/{x;/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/p}};d' $inputfile |  sed -e 's/BEGIN_EXCLUDE_LIST//' -e 's/END_EXCLUDE_LIST//' -e '/^$/d'`
    fIncluded=`sed '/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/{/BEGIN_INCLUDE_LIST/{h;d};H;/END_INCLUDE_LIST/{x;/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/p}};d' $inputfile |  sed -e 's/BEGIN_INCLUDE_LIST//' -e 's/END_INCLUDE_LIST//'  -e '/^$/d'`
    fExcludedFile=`sed '/BEGIN_FILE_EXCLUDE_LIST/,/END_FILE_EXCLUDE_LIST/{/BEGIN_FILE_EXCLUDE_LIST/{h;d};H;/END_FILE_EXCLUDE_LIST/{x;/BEGIN_FILE_EXCLUDE_LIST/,/END_FILE_EXCLUDE_LIST/p}};d' $inputfile |  sed -e 's/BEGIN_FILE_EXCLUDE_LIST//' -e 's/END_FILE_EXCLUDE_LIST//' -e '/^$/d'`
    fIncludedFile=`sed '/BEGIN_FILE_INCLUDE_LIST/,/END_FILE_INCLUDE_LIST/{/BEGIN_FILE_INCLUDE_LIST/{h;d};H;/END_FILE_INCLUDE_LIST/{x;/BEGIN_FILE_INCLUDE_LIST/,/END_FILE_INCLUDE_LIST/p}};d' $inputfile |  sed -e 's/BEGIN_FILE_INCLUDE_LIST//' -e 's/END_FILE_INCLUDE_LIST//'  -e '/^$/d'`

    fInstrumented=`pprof -l | grep -v "Reading" | grep -v ".TAU application"`

    # There might be spaces in the function names: change the separator
    IFS=$'\n'

    fIncluded="$(checkwildcard "$fIncluded")"
    incorrectInstrumentation=0

    for funcinclu in $fIncluded; do

        if echo $funcinclu | grep -qF "#"; then
            continue
        fi

        varinstrumented=1
        varexcluded=1
        varfileincluded=0
        varfileexcluded=1

        echo $fInstrumented | grep -qFw "$funcinclu"
        varinstrumented=$?
        echo $fExcluded | grep -qFw "$funcinclu"
        varexcluded=$?

        definition="$(echo $(symbols::file "$funcinclu") | sed "s:.*/::")"

        if [ $(echo $fIncludedFile | wc -w) -gt 0 ]; then
            echo $fIncludedFile | grep -qFw "$definition"
            varfileincluded=$?
        fi

        echo $fExcludedFile | grep -qFw "$definition"
        varfileexcluded=$?

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

    echo -n "Instrumentation of code with '$1':"
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
    runtest "$InputFile" "$Executable" "$OptionalC"
}
