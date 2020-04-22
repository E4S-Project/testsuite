import os
import sys
import subprocess
import random
import read_tau_data

class Plopper:
    def __init__(self,sourcefile,outputdir):

        # Initilizing global variables
        self.sourcefile = sourcefile
        self.outputdir = outputdir+"/tmp_files"

        if not os.path.exists(self.outputdir):
            os.makedirs(self.outputdir)


    #Creating a dictionary using parameter label and value
    def createDict(self, x, params):
        dictVal = {}
        for p, v in zip(params, x):
            dictVal[p] = v
        return(dictVal)

    #Replace the Markers in the source file with the corresponding Pragma values
    def plotValues(self, dictVal, inputfile, outputfile):
        with open(inputfile, "r") as f1:
            buf = f1.readlines()

        with open(outputfile, "w") as f2:
            for line in buf:
                modify_line = line
                for key, value in dictVal.items():
                    if key in modify_line:
                        if value != 'None': #For empty string options
                            modify_line = modify_line.replace('#'+key, str(value))

                if modify_line != line:
                    f2.write(modify_line)
                else:
                    #To avoid writing the Marker
                    f2.write(line)


    # Function to find the execution time of the interim file, and return the execution time as cost to the search module
    def findRuntime(self, x, params):
        interimfile = ""
        #exetime = float('inf')
        #exetime = sys.maxsize
        exetime = 1
        counter = random.randint(1, 10001) # To reduce collision increasing the sampling intervals

        interimfile = self.outputdir+"/"+str(counter)+".c"


        # Generate intermediate file
        dictVal = self.createDict(x, params)
        self.plotValues(dictVal, self.sourcefile, interimfile)

        #compile and find the execution time
        tmpbinary = interimfile[:-2]

        kernel_idx = self.sourcefile.rfind('/')
        kernel_dir = self.sourcefile[:kernel_idx]

        # Get the TAU-related environment variables
        tau = os.environ.get('TAU')
        if None == tau:
            printf( "The TAU environment variable must be defined" )
            return -1
        llvm = os.environ.get( 'LLVM_DIR' )
        if None == llvm:
            printf( "The LLVM_DIR environment variable must be defined" )
            return -1
        tau_makefile = os.environ.get( 'TAU_MAKEFILE' )
        if None == tau_makefile:
            printf( "The TAU_MAKEIFLE environment variable must be defined" )
            return -1
        function_file = os.environ.get( 'TAU_FUNCTIONS' )
        if None == function_file:
            printf( "The TAU_FUNCTIONS environment variable must be defined" )
            return -1

        taucmd = "-fplugin=" + llvm + "/lib/TAU_Profiling.so -mllvm " \
                 + "-tau-input-file=" + function_file + " -ldl " \
                 + "-L" + tau + "/lib/" + tau_makeifle + " -lTAU " \
                 + "-Wl,-rpath," + tau + "/lib/"+ tau_makeifle
        
        cmd1 = "clang -fno-caret-diagnostics " + taucmd + " "  +interimfile +" "  \
                " -DEXTRALARGE_DATASET -std=c99 -fno-unroll-loops -O3 -mllvm -polly -mllvm -polly-process-unprofitable -mllvm -polly-use-llvm-names -ffast-math -march=native -o "+tmpbinary

        cmd2 = "tau_exec -T shared,clang " + kernel_dir + "/exe.pl " +  tmpbinary

        #Find the compilation status using subprocess
        compilation_status = subprocess.run(cmd1, shell=True, stderr=subprocess.PIPE)

        #Find the execution time only when the compilation return code is zero, else return infinity
        if compilation_status.returncode == 0 :
            exetime = read_tau_data.getData( tau_functions, "." )  # /!\ careful: concurrency here
        else:
            print(compilation_status.stderr)
            print("compile failed")
        return exetime #return execution time as cost

