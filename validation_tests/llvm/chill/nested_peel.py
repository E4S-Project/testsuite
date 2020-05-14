
import chill

chill.source('src/mm.c')
chill.destination('src/mm_modified.c')
chill.procedure('mm')

chill.loop(0)

chill.known('ambn > 0')
chill.known('an > 0')
chill.known('bm > 0')

chill.peel( 0, 2, -4 ) # statement 0, loop 2 (middle, for j), 4 statements from END


