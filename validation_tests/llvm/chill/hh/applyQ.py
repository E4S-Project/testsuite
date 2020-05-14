import chill

chill.source( "src/applyQ.c" )
chill.destination( "src/applyQ_modified.c" )

chill.procedure( "applyQ" )

chill.loop( 0 )
chill.known( "len >= 0" )
chill.tile( 0, 1, 4 )

chill.loop( 1 )
chill.tile( 0, 1, 16 )

#chill.loop( 2 )
# Loop fusion etc here

