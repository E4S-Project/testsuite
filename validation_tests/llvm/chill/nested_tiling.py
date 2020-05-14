import chill

chill.source( 'src/nestedloops.c' )
chill.destination( 'src/nestedloops_modified.c' )
chill.procedure( 'foo' )
chill.loop( 0 )

chill.tile( 0, 2, 16 )
