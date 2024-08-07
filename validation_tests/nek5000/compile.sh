#!/bin/bash
. ./setup.sh
set -x
set -e
#cp -r $NEK5000_ROOT/bin/Nek5000/short_tests .
#cd short_tests
#export NEK_SOURCE_ROOT=
#export TOOLS_ROOT=$NEK5000_ROOT/bin/Nek5000/tools
#export TOOLS_BIN=$NEK5000_ROOT/bin/Nek5000/tools
#python -m 'unittest' NekTests.Eddy_EddyUv.test_PnPn2_Parallel
cp -r $NEK5000_ROOT/bin/Nek5000/tools .
mkdir -p bin
#ln -s $NEK5000_ROOT/bin/Nek5000/core/ ./core
mkdir -p core
cp $NEK5000_ROOT/bin/Nek5000/core/byte.c ./core
cd tools
./maketools genbox
./maketools genmap
cd ..
echo ./fdlf.box | ./bin/genbox
mv box.re2 fdlf.re2
genmap_input() {
    echo "fdlf"
    echo ""
}

genmap_input | ./bin/genmap
set +e
#Unclear why this fails...
makenek fdlf
set -e
make
