import chill

chill.source( "src/applyR.c" )
chill.destination( "src/applyR_modified.c" )

chill.procedure( "applyR" )

chill.loop( 1 )
chill.known( "len >= 0" )
chill.permute( [2, 1] )

chill.loop( 2 )
chill.permute( [2, 1] )
chill.tile( 0, 1, 16 )
# Loop fusion etc here

chill.loop( 3 )
chill.tile( 0, 1, 16 )

