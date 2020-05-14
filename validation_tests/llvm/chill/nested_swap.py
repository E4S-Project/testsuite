import chill

chill.source( 'src/nestedloops.c' )
chill.destination( 'src/nestedloops_modified.c' )
chill.procedure( 'foo' )
chill.loop( 0 )

chill.permute( [2,3,1] )
chill.unroll( 0, 3, 2 )
