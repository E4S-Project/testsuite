PROGRAM CALLF1
  IMPLICIT NONE

  INTERFACE
     FUNCTION toto( n )
       INTEGER, intent(in) :: n
       INTEGER :: toto
     END FUNCTION toto
  END INTERFACE
  

  INTEGER :: NB, RES
  NB = 8
  RES = toto( NB )

END PROGRAM
