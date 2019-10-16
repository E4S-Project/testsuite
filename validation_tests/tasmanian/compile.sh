#!/bin/bash
. ./setup.sh

g++ -O3 -std=c++11 -fopenmp -fPIC -Wstrict-aliasing -I${TASMANIAN_ROOT}/include -c gridtest_main.cpp -o gridtest_main.o
g++ -O3 -std=c++11 -fopenmp -fPIC -Wstrict-aliasing -I${TASMANIAN_ROOT}/include -c tasgridTestFunctions.cpp -o tasgridTestFunctions.o
g++ -O3 -std=c++11 -fopenmp -fPIC -Wstrict-aliasing -I${TASMANIAN_ROOT}/include -c tasgridExternalTests.cpp -o tasgridExternalTests.o
g++ -O3 -std=c++11 -fopenmp -fPIC -Wstrict-aliasing -I${TASMANIAN_ROOT}/include -c tasgridUnitTests.cpp -o tasgridUnitTests.o
g++ -O3 -std=c++11 -fopenmp -fPIC -Wstrict-aliasing -I${TASMANIAN_ROOT}/include -c tasgridTestInterfaceC.cpp -o tasgridTestInterfaceC.o
g++ -O3 -std=c++11 -fopenmp -fPIC -Wstrict-aliasing -L${TASMANIAN_ROOT}/lib \
gridtest_main.o tasgridTestFunctions.o tasgridExternalTests.o tasgridUnitTests.o tasgridTestInterfaceC.o \
-o gridtest -ltasmaniansparsegrid -lm


mpic++ -O3 -std=c++11 -fopenmp -fPIC -Wstrict-aliasing -I../SparseGrids -I${TASMANIAN_ROOT}/include -c tasdream_main.cpp -o tasdream_main.o
mpic++ -O3 -std=c++11 -fopenmp -fPIC -Wstrict-aliasing -I../SparseGrids -I${TASMANIAN_ROOT}/include -c tasdreamBenchmark.cpp -o tasdreamBenchmark.o
mpic++ -O3 -std=c++11 -fopenmp -fPIC -Wstrict-aliasing -I../SparseGrids -I${TASMANIAN_ROOT}/include -c tasdreamExternalTests.cpp -o tasdreamExternalTests.o
mpic++ -O3 -std=c++11 -fopenmp -fPIC -Wstrict-aliasing -I../SparseGrids -I${TASMANIAN_ROOT}/include -c tasdreamTestPDFs.cpp -o tasdreamTestPDFs.o
mpic++ -O3 -std=c++11 -fopenmp -fPIC -Wstrict-aliasing -I../SparseGrids -I${TASMANIAN_ROOT}/include -c TasmanianDREAM.cpp -o TasmanianDREAM.o
mpic++ -O3 -std=c++11 -fopenmp -fPIC -Wstrict-aliasing -I../SparseGrids -I${TASMANIAN_ROOT}/include -c tdrCorePDF.cpp -o tdrCorePDF.o

mpic++ -O3 -std=c++11 -fopenmp -fPIC -Wstrict-aliasing -L${TASMANIAN_ROOT}/lib \
tasdream_main.o tasdreamBenchmark.o TasmanianDREAM.o tdrCorePDF.o tasdreamExternalTests.o tasdreamTestPDFs.o \
-o tasdream -ltasmaniandream -ltasmaniansparsegrid -lm
