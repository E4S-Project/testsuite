import chill

chill.source( "householder2.c" )
chill.destination( "householder2_modified.c" )

chill.procedure( "matmul" )
chill.loop( 0 )

print( "function 1")

chill.known( "M >= 0" )
chill.known( "K >= 0" )
chill.known( "N >= 0" )

chill.permute( [1, 3, 2] )
chill.tile( 0, 2, 16 )

chill.procedure( "applyR" )
chill.known( "len >= 0" )

chill.loop( 1 )
chill.permute( [2, 1] )

chill.loop( 2 )
chill.permute( [2, 1] )
chill.tile( 0, 1, 16 )
# Loop fusion etc here

chill.loop( 3 )
chill.tile( 0, 1, 16 )

chill.procedure( "applyQ" )
chill.known( "len >= 0" )

chill.loop( 0 )
chill.tile( 0, 1, 4 )

chill.loop( 1 )
chill.tile( 0, 1, 16 )

#chill.loop( 2 )
# Loop fusion etc here

chill.original()
chill.print_code()
