#!/usr/bin/env python

import os
import sys
import commands

flang = "flang"

def compileWithStaticLib():

    # Compile once, with -v
    command = flang + " -v " + " ".join( sys.argv[1:] )
    status, output = commands.getstatusoutput( command )

    # Find the libraries
    staticlibs = []
    for a in sys.argv[1:]:
        if a.endswith( '.a' ) or a.startswith( "-l" ):
            staticlibs.append( a )

    # Find the link edition command
    lines = output.split( '\n' )
    fc = os.environ[ 'F18_FC' ]

    for l in lines:
        if staticlibs[0] in l and fc in l and l.startswith( "Driving" ):
            # Add the static libs at the end and remove -v
            fullstr = l.split()
            cnt = 0
            while fullstr[ cnt ] != fc:
                cnt = cnt + 1

            c = []
            for elem in fullstr[cnt:]:
                if elem != "-v":
                    c.append( elem )

            command = " ".join( c ) + " " + " ".join( staticlibs )
#            print( command )
            return command

    # We shouldn't arrive here
    print( "Could not build the command to execute to link against static libs", staticlibs )
    return ""

def main():
    
    # Is there a static lib somewhere in the command line?
    staticlib = False
    for a in sys.argv[1:]:
        if a.endswith( '.a' ) or a.startswith( "-l" ):
            staticlib = True

    if not staticlib:
        command = flang + " " + " ".join( sys.argv[1:] )
    else:
        command = compileWithStaticLib()

    stream = os.popen( command )
    output = stream.read()
    output

    return

if "__main__" ==  __name__:
    main()
