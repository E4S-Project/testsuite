#!/usr/bin/env python3

from ninja_syntax import Writer

with open( "build.ninja", "w" ) as buildfile:
    n = Writer( buildfile )

    n.variable( "cc", "clang" )
    n.variable( "cflags", "-Weverything" )

    n.rule( "compile", command = "$cc $cflags -c $in -o $out",
                       description = "Compiling object file $out" )

    n.rule( "link", command = "$cc $in -o $out",
                       description = "Linking executable $out" )
    n.build( "hello.o", "compile hello.c" )
    n.build( "hello", "link hello.o" )

    n.default( "hello" )
