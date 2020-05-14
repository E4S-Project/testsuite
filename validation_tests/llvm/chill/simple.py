import chill

chill.source( 'src/singleloop.c' )
chill.destination( 'src/singleloop_modified.c' )
chill.procedure( 'foo' )
chill.loop( 0 )

chill.original()
chill.print_code()
