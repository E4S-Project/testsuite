import chill

chill.source( 'src/nestedloops.c' )
chill.destination( 'src/nestedloops_modified.c' )
chill.procedure( 'foo' )
chill.loop( 0 )

#chill.original()
#chill.print_code()
