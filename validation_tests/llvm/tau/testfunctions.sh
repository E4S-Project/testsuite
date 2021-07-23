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

    # Get the llvm version from llvm-config
    export TEST_LLVM_VERSION_MAJOR=$(echo `$TEST_LLVM_INSTALL/bin/llvm-config --version | awk -F"." {'print $1'}`)

    # Is the plugin installed somewhere else ?
    export TEST_PLUGIN_PREFIX=$([ -n "$PLUGIN_DIR" ] && echo $PLUGIN_DIR || echo $LLVM_INSTALL/lib)

    export TEST_TAU_BASEDIR=$TAU_BASEDIR
    export TEST_TAU_INSTALL=$TEST_TAU_BASEDIR/lib/$TAU_INSTALL

    export TEST_OLD_PATH=$PATH
    export PATH=$TAU_BASEDIR/bin:$PATH

    if [ -z "$TEST_LLVM_INSTALL" -o -z "$TEST_PLUGIN_PREFIX" -o -z "$TEST_TAU_INSTALL" -o -z "$TEST_TAU_BASEDIR" ] ; then
        output::err "Invalid parameters."
        output::err Using LLVM: \"$TEST_LLVM_INSTALL\"
        output::err Using plugin in: \"$TEST_PLUGIN_PREFIX\"
        output::err Using libTAU.so from: \"$TEST_TAU_INSTALL\"
        output::err If any of those values are erroneous, please set the
        output::err LLVM, PLUGIN_DIR, TAU_BASEDIR or TAU_INSTALL
        output::err environment variables.
        exit 1
    fi
}

environment::exit() {
    export PATH=$TEST_OLD_PATH
    unset TEST_LLVM_INSTALL TEST_PLUGIN_PREFIX TEST_TAU_INSTALL TEST_TAU_BASEDIR TEST_LLVM_VERSION_MAJOR TEST_OLD_PATH
}

output::err() {
echo "[$(date +'%Y-%m-%dT%H:%M:%S%z')]: $*" >&2
}

output::status() {
if [ -n "$QUIET_TEST_OUTPUT" ] ; then
    return
fi

if [ "$2" -ne 0 ] ; then
    echo -ne "$BRED"
else
    echo -ne "$BGREEN"
fi

echo -e "$1: $2 $NC"
}

output::assert() {
COLS=`tput cols 2>/dev/null || echo 80`
PASSED="[PASSED]"
FAILED="[FAILED]"

PROMPT_LEN=${#1}
ASSERT_LEN=${#PASSED}

SEPARATOR=$(head -c $(($COLS - $PROMPT_LEN - $ASSERT_LEN)) < /dev/zero | tr '\0' '\ ')

if [ "$2" -eq 0 ] ; then
   echo -e "$1$SEPARATOR$BGREEN$PASSED$NC" 
else 
   echo -e "$1$SEPARATOR$BRED$FAILED$NC" 
fi
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
        | sed 's/:[^:]*$//'                   `# Remove the line number` \
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

test::compile() {
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
    if [ $TEST_LLVM_VERSION_MAJOR -gt 12 ]; then
        NPM_LOAD="-fpass-plugin=${PLUGIN}"
    fi

    ERRFILE=`mktemp`
#missing $NPM_LOAD
#added -fpass-plugin=$PLUGIN
    $COMPILER $NPM_LOAD -o $OUTPUT \
        -O3 -g \
        -fplugin=$PLUGIN \
        -mllvm \
        -tau-input-file=$FUNC_LIST \
        -L$TEST_TAU_INSTALL \
        -ldl -lTAU -lm \
        -Wl,-rpath,$TEST_TAU_INSTALL \
        $SOURCES \
       # &> $ERRFILE
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

test::run() {
    environment::enter
    FUNC_LIST=$1
    EXECUTABLE=$2
    OptionalC=${3:-C++}
    
    OUTFILE=`mktemp`
    ERRFILE=`mktemp`
    
    rm -f profile.*

    tau_exec -T serial "./$EXECUTABLE" 256 256 > $OUTFILE 2> $ERRFILE
    SUCCESS=$?
    
    output::status "Execution of $EXECUTABLE" $SUCCESS
    
    if [ $SUCCESS -ne 0 ] ; then
        cat "$ERRFILE"
        exit 1
    fi
    
    test::verify $FUNC_LIST $OptionalC

    rm -f $OUTFILE $ERRFILE profile.*
    environment::exit
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

# Compares instrumentation instructions with actual instrumentation.
# 
# For every function listed as included, we look if:
#   - it is implemented
#   - it is excluded
#   - his file is included
#   - his file is excluded
#
# Depending on these caracteristics, it concludes whether the
# instrumentation (or non-instrumentation) is lawful
#
test::verify() {
    inputfile=$1
    OptionalC=${2:-C++}

    if [ $(find . -name $inputfile | wc -l) -eq 0 ]; then
        output::err "Input file not found: stopping the verification"
        exit 1
    elif [ $(find . -name profile.* | wc -l) -eq 0 ]; then
        output::err "Profile file not found: stopping the verification"
        exit 1
    fi

    # Parses the input file for inclusion/exclusion information
    fExcluded=`sed '/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/{/BEGIN_EXCLUDE_LIST/{h;d};H;/END_EXCLUDE_LIST/{x;/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/p}};d' $inputfile |  sed -e 's/BEGIN_EXCLUDE_LIST//' -e 's/END_EXCLUDE_LIST//' -e '/^$/d'`
    fIncluded=`sed '/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/{/BEGIN_INCLUDE_LIST/{h;d};H;/END_INCLUDE_LIST/{x;/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/p}};d' $inputfile |  sed -e 's/BEGIN_INCLUDE_LIST//' -e 's/END_INCLUDE_LIST//'  -e '/^$/d'`
    fExcludedFile=`sed '/BEGIN_FILE_EXCLUDE_LIST/,/END_FILE_EXCLUDE_LIST/{/BEGIN_FILE_EXCLUDE_LIST/{h;d};H;/END_FILE_EXCLUDE_LIST/{x;/BEGIN_FILE_EXCLUDE_LIST/,/END_FILE_EXCLUDE_LIST/p}};d' $inputfile |  sed -e 's/BEGIN_FILE_EXCLUDE_LIST//' -e 's/END_FILE_EXCLUDE_LIST//' -e '/^$/d'`
    fIncludedFile=`sed '/BEGIN_FILE_INCLUDE_LIST/,/END_FILE_INCLUDE_LIST/{/BEGIN_FILE_INCLUDE_LIST/{h;d};H;/END_FILE_INCLUDE_LIST/{x;/BEGIN_FILE_INCLUDE_LIST/,/END_FILE_INCLUDE_LIST/p}};d' $inputfile |  sed -e 's/BEGIN_FILE_INCLUDE_LIST//' -e 's/END_FILE_INCLUDE_LIST//'  -e '/^$/d'`

    # Parses the output of pprof for instrumentation information
    fInstrumented=`pprof -l | grep -v "Reading" | grep -v ".TAU application"`


    # There might be spaces in the function names: change the separator
    IFS=$'\n'

    # Updates the list of included and excluded functions in case of wildcards
    fIncluded="$(checkwildcard "$fIncluded")"
    fExcluded="$(checkwildcard "$fExcluded")"
    incorrectInstrumentation=0

    # Main loop through included functions
    for funcinclu in $fIncluded; do

        if echo $funcinclu | grep -qF "#"; then
            continue
        fi
        
        # Determines the file where the current function was implemented
        definition="$(echo $(symbols::file "$funcinclu") | sed "s:.*/::")"

        # Initialises the characteristics, 0 meaning true, 1 meaning false
        varinstrumented=1
        varexcluded=1
        varfileincluded=0
        varfileexcluded=1


        # Checks the inclusion/exclusion status of the function
        echo $fInstrumented | grep -qFw "$funcinclu"
        varinstrumented=$?
                echo $fExcluded | grep -qFw "$funcinclu"
        varexcluded=$?
        # If no file is listed as included, all files are
        if [ $(echo $fIncludedFile | wc -w) -gt 0 ]; then
            echo $fIncludedFile | grep -qFw "$definition"
            varfileincluded=$?
        fi
        echo $fExcludedFile | grep -qFw "$definition"
        varfileexcluded=$?

        # Compares the values to determine lawful or unlawful instrumentation/non-instrumentation
        if [ $varinstrumented -eq 0 ] && [ ! $varexcluded -eq 0 ] && [ $varfileincluded -eq 0 ] && [ ! $varfileexcluded -eq 0 ];
        then
            output::status "$funcinclu: Lawfully instrumented" 0
        elif [ $varinstrumented -eq 1 ] && [ ! $varexcluded -eq 0 ] && [ $varfileincluded -eq 0 ] && [ ! $varfileexcluded -eq 0 ];
        then
            ((incorrectInstrumentation=incorrectInstrumentation+1))
            output::status "$funcinclu: Wrongfully not instrumented: included and not excluded" 1
        elif [ $varinstrumented -eq 0 ] && [ ! $varexcluded -eq 1 ];
        then
            ((incorrectInstrumentation=incorrectInstrumentation+1))
            output::status "$funcinclu: Wrongfully instrumented: excluded" 1
        elif [ $varinstrumented -eq 0 ] && [ $varfileincluded -eq 1 ];
        then
            ((incorrectInstrumentation=incorrectInstrumentation+1))
            output::status "$funcinclu: Wrongfully instrumented: source file is not included" 1
        elif [ $varinstrumented -eq 0 ] && [ ! $varfileexcluded -eq 1 ];
        then
            ((incorrectInstrumentation=incorrectInstrumentation+1))
            output::status "$funcinclu: Wrongfully instrumented: source file is excluded" 1
        elif [ $varinstrumented -eq 1 ] && ([ ! $varexcluded -eq 1 ] || [ $varfileincluded -eq 1 ] || [ ! $varfileexcluded -eq 1 ]);
        then
            output::status "$funcinclu: Lawfully not instrumented: excluded or not included" 0
        else
            echo Uncovered case to implement
            ((incorrectInstrumentation=incorrectInstrumentation+1))
        fi

    done

    # Loops through instrumented function
    # Checks they were initialy included
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

    output::assert "Instrumentation of code with '$1':" $incorrectInstrumentation
}
