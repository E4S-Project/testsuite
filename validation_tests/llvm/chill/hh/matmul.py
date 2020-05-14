import chill

chill.source( "src/matmul.c" )
chill.destination( "src/matmul_modified.c" )

chill.procedure( "matmul" )
chill.loop( 0 )

#chill.known( "M >= 0" )
#chill.known( "K >= 0" )
#chill.known( "N >= 0" )

#chill.permute( [1, 3, 2] )
#chill.tile( 0, 2, 16 )

